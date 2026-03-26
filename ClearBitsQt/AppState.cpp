#include "AppState.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QIODevice>
#include <QStandardPaths>
#include <QTextStream>

#include <cderr.h>
#include <commdlg.h>

#include <ctime>
#include <cstring>

namespace
{
constexpr long kAlgoChangeRewindSeconds = 2;

QString openFileDialogErrorMessage(DWORD errorCode)
{
    switch (errorCode) {
    case FNERR_BUFFERTOOSMALL:
        return QStringLiteral("The selection is too large for the file dialog buffer. "
                              "Select fewer files or use shorter paths.");
    case FNERR_INVALIDFILENAME:
        return QStringLiteral("The file dialog reported an invalid file name.");
    case FNERR_SUBCLASSFAILURE:
        return QStringLiteral("The file dialog could not initialize correctly.");
    case CDERR_INITIALIZATION:
        return QStringLiteral("The file dialog failed to initialize.");
    case CDERR_FINDRESFAILURE:
    case CDERR_LOADRESFAILURE:
    case CDERR_LOADSTRFAILURE:
    case CDERR_LOCKRESFAILURE:
        return QStringLiteral("The file dialog could not load required resources.");
    case CDERR_MEMALLOCFAILURE:
    case CDERR_MEMLOCKFAILURE:
        return QStringLiteral("The file dialog ran out of memory.");
    case CDERR_STRUCTSIZE:
        return QStringLiteral("The file dialog was called with an invalid structure size.");
    default:
        return QStringLiteral("The file dialog failed with error 0x%1.")
            .arg(errorCode, 0, 16).toUpper();
    }
}
}

// ---------------------------------------------------------------------------
// Static waveOut callback — called from an audio thread.
// Posts a WomDoneEvent to AppState so playBuffer() runs on the Qt main thread.
// ---------------------------------------------------------------------------
void CALLBACK AppState::waveOutCallback(HWAVEOUT /*hwo*/, UINT uMsg,
                                        DWORD_PTR dwInstance,
                                        DWORD_PTR dwParam1,
                                        DWORD_PTR /*dwParam2*/)
{
    if (uMsg == WOM_DONE) {
        WAVEHDR *hdr = reinterpret_cast<WAVEHDR *>(dwParam1);
        if (hdr->dwFlags & WHDR_DONE) {
            auto *sb   = reinterpret_cast<CSampleBuffer *>(hdr->dwUser);
            auto *self = reinterpret_cast<AppState *>(dwInstance);
            QCoreApplication::postEvent(self, new WomDoneEvent(sb));
        }
    }
}

// ---------------------------------------------------------------------------
// Constructor / destructor
// ---------------------------------------------------------------------------
AppState::AppState(QObject *parent)
    : QObject(parent)
{
    loadStartupPlaylist();

    m_progressTimer.setInterval(500);
    connect(&m_progressTimer, &QTimer::timeout, this, &AppState::updateProgressText);

    m_waveReader.Create();
    CryptAcquireContext(&m_hProvider, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

    // Seed random number generator for rand() algo
    srand(static_cast<unsigned>(time(nullptr)));

    // Load random.org data for random.org algo
    QFile f(":/random.bin");
    if (f.open(QIODevice::ReadOnly))
        m_randomOrgData = f.readAll();

    // Open audio device and pre-fill buffers with silence (no file open yet).
    if (openOutDevice(nullptr)) {
        for (int i = 0; i < NUM_SAMPLE_BUFFERS; ++i) {
            m_arrSB[i].Alloc(MAX_SOUNDBUF_SIZE);
            playBuffer(&m_arrSB[i]);
        }
    }
}

AppState::~AppState()
{
    // Persist playlist for next session.
    {
        const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QDir().mkpath(dirPath);
        QFile f(dirPath + QStringLiteral("/save_tracklist.txt"));
        if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&f);
            for (const QString &entry : std::as_const(m_playlistEntries))
                out << entry << '\n';
        }
    }

    // Stop playback and clean up.
    if (m_hProvider) {
        CryptReleaseContext(m_hProvider, 0);
        m_hProvider = 0;
    }

    m_waveReader.Close();

    if (m_hWaveOut) {
        waveOutReset(m_hWaveOut);

        for (int i = 0; i < NUM_SAMPLE_BUFFERS; ++i)
            m_arrSB[i].UnprepareOut(m_hWaveOut);

        waveOutClose(m_hWaveOut);
        m_hWaveOut = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Properties
// ---------------------------------------------------------------------------
QStringList AppState::playlistEntries() const { return m_playlistEntries; }
bool        AppState::playing()          const { return m_playing; }
int         AppState::selectedIndex()    const { return m_selectedIndex; }
int         AppState::algo()             const { return m_nAlgo; }
QString     AppState::progressText()     const { return m_progressText; }

void AppState::setPlaying(bool playing)
{
    if (m_playing == playing)
        return;
    m_playing = playing;
    emit playingChanged();

    if (playing) {
        m_progressTimer.start();
        updateProgressText();
    } else {
        m_progressTimer.stop();
    }
}

void AppState::setSelectedIndex(int index)
{
    if (m_selectedIndex == index)
        return;
    m_selectedIndex = index;
    emit selectedIndexChanged();

    if (m_waveReader.IsOpen()) {
        stop();
        if (m_selectedIndex >= 0 && m_selectedIndex < m_playlistEntries.size())
            play();
    }
}

void AppState::setAlgo(int algo)
{
    if (m_nAlgo == algo)
        return;

    if (m_hWaveOut) {
        if (m_playing && m_waveReader.IsOpen()) {
            long newPosition = playbackPositionBytes()
                             - static_cast<long>(m_wfx.nAvgBytesPerSec * kAlgoChangeRewindSeconds);
            if (newPosition < 0)
                newPosition = 0;

            m_waveReader.Seek(newPosition);
            m_playbackPositionBytes = newPosition;
            updateProgressText();
        }

        waveOutReset(m_hWaveOut);   // Match MFC: refill buffers immediately using the new algorithm.
    }

    m_nAlgo = algo;
    emit algoChanged();
}

// ---------------------------------------------------------------------------
// togglePlaying — called from QML Play/Pause button
// ---------------------------------------------------------------------------
void AppState::togglePlaying()
{
    if (m_waveReader.IsOpen())
        pause();
    else
        play();
}

void AppState::seekRelativeSeconds(int seconds)
{
    if (m_wfx.nAvgBytesPerSec == 0)
        return;

    const qint64 deltaBytes = static_cast<qint64>(m_wfx.nAvgBytesPerSec) * seconds;

    if (m_waveReader.IsOpen()) {
        qint64 newPosition = static_cast<qint64>(playbackPositionBytes()) + deltaBytes;
        if (newPosition < 0)
            newPosition = 0;

        if (!m_waveReader.Seek(static_cast<LONG>(newPosition)))
            return;

        if (m_hWaveOut) {
            waveOutReset(m_hWaveOut);
            m_playbackPositionBytes = static_cast<long>(newPosition);
            setProgressTextForBytes(m_playbackPositionBytes);
        }
        return;
    }

    if (!m_pauseFile.isEmpty()) {
        qint64 newPosition = static_cast<qint64>(m_pausePos) + deltaBytes;
        if (newPosition < 0)
            newPosition = 0;

        m_pausePos = static_cast<long>(newPosition);
        m_playbackPositionBytes = m_pausePos;
        setProgressTextForBytes(m_pausePos);
    }
}

void AppState::prevTrack()
{
    if (m_selectedIndex <= 0)
        return;

    bool wasPlaying = m_waveReader.IsOpen();
    stop();
    setSelectedIndex(m_selectedIndex - 1);
    if (wasPlaying)
        play();
}

void AppState::nextTrack()
{
    if (m_selectedIndex >= m_playlistEntries.size() - 1)
        return;

    bool wasPlaying = m_waveReader.IsOpen();
    stop();
    setSelectedIndex(m_selectedIndex + 1);
    if (wasPlaying)
        play();
}

void AppState::loadPlaylist()
{
    wchar_t szFile[32768] = {};
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = L"Audio Files\0*.wav;*.mp3\0Playlists\0*.m3u\0All Files\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = _countof(szFile);
    ofn.Flags = OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST
              | OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

    if (!GetOpenFileNameW(&ofn)) {
        const DWORD errorCode = CommDlgExtendedError();
        if (errorCode != 0) {
            const QString message = openFileDialogErrorMessage(errorCode);
            MessageBoxW(nullptr,
                        reinterpret_cast<LPCWSTR>(message.utf16()),
                        L"Load Playlist",
                        MB_OK | MB_ICONERROR);
        }
        return;
    }

    stop();
    m_playlistEntries.clear();
    if (m_selectedIndex != -1) {
        m_selectedIndex = -1;
        emit selectedIndexChanged();
    }

    auto appendM3uEntries = [this](const QString &playlistPath) {
        QDir m3uDir = QFileInfo(playlistPath).absoluteDir();
        QFile f(playlistPath);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&f);
            while (!stream.atEnd()) {
                QString line = stream.readLine().trimmed();
                if (line.isEmpty() || line.startsWith(QLatin1Char('#')))
                    continue;

                QString entry = QDir::fromNativeSeparators(line);
                if (QFileInfo(entry).isRelative())
                    entry = QDir::fromNativeSeparators(m3uDir.absoluteFilePath(entry));
                m_playlistEntries.append(entry);
            }
        }
    };

    const wchar_t *first = szFile;
    const wchar_t *second = first + wcslen(first) + 1;
    if (*second == L'\0') {
        QString path = QDir::fromNativeSeparators(QString::fromWCharArray(first));
        if (path.endsWith(".m3u", Qt::CaseInsensitive))
            appendM3uEntries(path);
        else
            m_playlistEntries.append(path);
    } else {
        const QString directory = QDir::fromNativeSeparators(QString::fromWCharArray(first));
        for (const wchar_t *name = second; *name != L'\0'; name += wcslen(name) + 1) {
            const QString path = QDir::fromNativeSeparators(
                QDir(directory).absoluteFilePath(QString::fromWCharArray(name)));
            if (path.endsWith(".m3u", Qt::CaseInsensitive))
                appendM3uEntries(path);
            else
                m_playlistEntries.append(path);
        }
    }

    emit playlistEntriesChanged();

    if (!m_playlistEntries.isEmpty()) {
        m_selectedIndex = 0;
        emit selectedIndexChanged();
    }
}

void AppState::clearPlaylist()
{
    stop();
    m_playlistEntries.clear();
    emit playlistEntriesChanged();
    if (m_selectedIndex != -1) {
        m_selectedIndex = -1;
        emit selectedIndexChanged();
    }
}

// ---------------------------------------------------------------------------
// event — handle WomDoneEvent on the main thread
// ---------------------------------------------------------------------------
bool AppState::event(QEvent *e)
{
    if (e->type() == WomDoneEvent::Type) {
        playBuffer(static_cast<WomDoneEvent *>(e)->sampleBuffer());
        return true;
    }
    return QObject::event(e);
}

// ---------------------------------------------------------------------------
// Audio device
// ---------------------------------------------------------------------------
bool AppState::openOutDevice(WAVEFORMATEX *pwfx)
{
    WAVEFORMATEX wfStandard = {};
    if (!pwfx) {
        wfStandard.wFormatTag      = WAVE_FORMAT_PCM;
        wfStandard.nChannels       = 2;
        wfStandard.wBitsPerSample  = 16;
        wfStandard.nSamplesPerSec  = 44100;
        wfStandard.nBlockAlign     = (wfStandard.wBitsPerSample / 8) * wfStandard.nChannels;
        wfStandard.nAvgBytesPerSec = wfStandard.nSamplesPerSec * wfStandard.nBlockAlign;
        wfStandard.cbSize          = 0;
        pwfx = &wfStandard;
    }

    if (m_hWaveOut) {
        // Already open — skip if format hasn't changed.
        if (memcmp(pwfx, &m_wfx, sizeof(WAVEFORMATEX)) == 0)
            return true;
        closeOutDevice();
    }

    MMRESULT result = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, pwfx,
                                  reinterpret_cast<DWORD_PTR>(&AppState::waveOutCallback),
                                  reinterpret_cast<DWORD_PTR>(this),
                                  CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR) {
        m_hWaveOut = nullptr;
        return false;
    }

    m_wfx = *pwfx;
    return true;
}

void AppState::closeOutDevice()
{
    if (!m_hWaveOut)
        return;

    waveOutReset(m_hWaveOut);

    for (int i = 0; i < NUM_SAMPLE_BUFFERS; ++i)
        m_arrSB[i].UnprepareOut(m_hWaveOut);

    waveOutClose(m_hWaveOut);
    m_hWaveOut = nullptr;
}

// ---------------------------------------------------------------------------
// playBuffer — fills one buffer from the open file (or silence) and submits it
// ---------------------------------------------------------------------------
void AppState::playBuffer(CSampleBuffer *pSB)
{
    if (!m_hWaveOut)
        return;

    LONG lBufSize = getNextBufSize();

    pSB->UnprepareOut(m_hWaveOut);
    pSB->PrepareOut(m_hWaveOut, lBufSize);

    LONG lBytesFilled = 0;

    if (m_waveReader.IsOpen()) {
        DWORD dwWritten = 0;
        if (m_waveReader.Read(static_cast<DWORD>(lBufSize),
                              reinterpret_cast<BYTE *>(pSB->m_pData),
                              &dwWritten)) {
            lBytesFilled = static_cast<LONG>(dwWritten);

            if (lBytesFilled < lBufSize) {
                const bool reachedEnd = m_waveReader.IsPcm() || m_waveReader.FoundEnd();
                if (reachedEnd) {
                    // PCM short reads mean EOF. For MP3, only stop once the decoder has
                    // actually reported end-of-file; short fills after a seek can be transient.
                    m_waveReader.Close();
                    m_pauseFile.clear();    // don't leak pause state across track boundary
                    m_pausePos = 0;

                    if (m_selectedIndex < m_playlistEntries.size() - 1) {
                        ++m_selectedIndex;
                        // Don't emit selectedIndexChanged() yet for the gapless path —
                        // deferred until the old-track tail is audibly finished.
                        const HWAVEOUT oldDevice = m_hWaveOut;
                        if (play(/*resetDevice=*/false)) {
                            if (m_hWaveOut != oldDevice) {
                                // Format change: device was reset, hardware counter is 0,
                                // m_playbackPositionBytes = 0 (from play()) is correct.
                                // No tail in the buffer — emit UI updates immediately.
                                emit selectedIndexChanged();
                                setProgressTextForBytes(0);
                                // Re-prepare pSB for the new device and fill a full fresh buffer.
                                pSB->PrepareOut(m_hWaveOut, lBufSize);
                                lBytesFilled = 0;
                            } else {
                                // Gapless same-device advance. Rebase m_playbackPositionBytes so
                                // playbackPositionBytes() = 0 when the new track becomes audible.
                                // The new track starts lBytesFilled bytes into the not-yet-submitted
                                // buffer, so subtract both the hardware position and the tail ahead.
                                MMTIME mmt = {};
                                mmt.wType = TIME_BYTES;
                                if (waveOutGetPosition(m_hWaveOut, &mmt, sizeof(mmt)) == MMSYSERR_NOERROR
                                        && mmt.wType == TIME_BYTES)
                                    m_playbackPositionBytes = -static_cast<long>(mmt.u.cb) - lBytesFilled;
                                else
                                    m_playbackPositionBytes = 0;

                                // Defer UI updates until the old-track tail has drained.
                                // Capture bytes/sec before play() — same format is guaranteed
                                // in the same-device path, but capturing first is explicit.
                                const DWORD bytesPerSec = m_wfx.nAvgBytesPerSec;
                                const int tailMs = (bytesPerSec > 0)
                                    ? static_cast<int>(lBytesFilled * 1000LL / bytesPerSec)
                                    : 0;
                                const int gen = ++m_advanceGeneration;
                                m_suppressProgressUpdate = true;
                                QTimer::singleShot(tailMs, this, [this, gen]() {
                                    if (m_advanceGeneration != gen)
                                        return;  // stale: stop() already cleared m_suppressProgressUpdate
                                    m_suppressProgressUpdate = false;
                                    emit selectedIndexChanged();
                                    setProgressTextForBytes(0);
                                });
                            }
                            // Fill remainder (or full buffer on format change) from the new track.
                            DWORD dwWritten2 = 0;
                            if (m_waveReader.Read(static_cast<DWORD>(lBufSize - lBytesFilled),
                                                  reinterpret_cast<BYTE *>(pSB->m_pData + lBytesFilled),
                                                  &dwWritten2))
                                lBytesFilled += static_cast<LONG>(dwWritten2);
                        } else {
                            emit selectedIndexChanged();  // m_selectedIndex advanced; keep UI consistent
                            setPlaying(false);
                        }
                    } else {
                        setPlaying(false);
                    }
                }
            }
        }
    }

    // Zero-fill any unfilled portion.
    if (lBytesFilled < lBufSize)
        ZeroMemory(pSB->m_pData + lBytesFilled, lBufSize - lBytesFilled);

    MMRESULT result = waveOutWrite(m_hWaveOut, pSB->m_pWaveHdr, sizeof(WAVEHDR));
    Q_UNUSED(result);
}

LONG AppState::getNextBufSize_Fixed()
{
    // 2-second fixed buffer
    WORD wSample  = 0x8000;
    LONG lBufSize = (m_wfx.nAvgBytesPerSec * 2) + (wSample * m_wfx.nBlockAlign);
    return lBufSize;
}

LONG AppState::getNextBufSize()
{
    switch (m_nAlgo) {
    case ALGO_VC:
        return getNextBufSize_VC();
    case ALGO_CAPI:
        return getNextBufSize_CAPI();
    case ALGO_RANDOM_ORG:
        return getNextBufSize_RandomOrg();
    default:
        return getNextBufSize_Fixed();
    }
}

LONG AppState::getNextBufSize_VC()
{
    WORD wSample = static_cast<WORD>((rand() << 1) | (rand() & 0x0001));
    return (m_wfx.nAvgBytesPerSec * 2) + (wSample * m_wfx.nBlockAlign);
}

LONG AppState::getNextBufSize_CAPI()
{
    WORD wSample = 0;
    if (m_hProvider)
        CryptGenRandom(m_hProvider, sizeof(wSample), reinterpret_cast<BYTE *>(&wSample));
    return (m_wfx.nAvgBytesPerSec * 2) + (wSample * m_wfx.nBlockAlign);
}

LONG AppState::getNextBufSize_RandomOrg()
{
    if (m_randomOrgData.size() < static_cast<int>(sizeof(WORD)))
        return getNextBufSize_Fixed();

    WORD wSample;
    memcpy(&wSample, m_randomOrgData.constData() + m_randomOrgPos, sizeof(wSample));
    m_randomOrgPos += static_cast<int>(sizeof(wSample));
    if (m_randomOrgPos + static_cast<int>(sizeof(wSample)) > m_randomOrgData.size())
        m_randomOrgPos = 0;

    wSample = MAKEWORD(HIBYTE(wSample), LOBYTE(wSample));

    return (m_wfx.nAvgBytesPerSec * 2) + (wSample * m_wfx.nBlockAlign);
}

long AppState::playbackPositionBytes() const
{
    if (!m_hWaveOut)
        return m_playbackPositionBytes;

    MMTIME mmTime = {};
    mmTime.wType = TIME_BYTES;
    if (waveOutGetPosition(m_hWaveOut, &mmTime, sizeof(mmTime)) != MMSYSERR_NOERROR)
        return m_playbackPositionBytes;

    if (mmTime.wType != TIME_BYTES)
        return m_playbackPositionBytes;

    // Clamp to 0: can briefly go negative during a gapless advance while the
    // hardware is still playing the old-track tail in the mixed buffer.
    const long pos = m_playbackPositionBytes + static_cast<long>(mmTime.u.cb);
    return pos < 0 ? 0 : pos;
}

void AppState::setProgressTextForBytes(long bytes)
{
    int totalSecs = static_cast<int>(bytes / m_wfx.nAvgBytesPerSec);
    int mins = totalSecs / 60;
    int secs = totalSecs % 60;

    QString text = QString::number(mins) + ":"
        + QString::number(secs).rightJustified(2, QLatin1Char('0'));

    if (m_progressText != text) {
        m_progressText = text;
        emit progressTextChanged();
    }
}

void AppState::updateProgressText()
{
    if (m_suppressProgressUpdate)
        return;
    if (!m_playing || m_wfx.nAvgBytesPerSec == 0)
        return;

    setProgressTextForBytes(playbackPositionBytes());
}

// ---------------------------------------------------------------------------
// Play / Pause / Stop
// ---------------------------------------------------------------------------
bool AppState::play(bool resetDevice)
{
    if (m_waveReader.IsOpen())
        return true;   // Already playing.

    if (m_playlistEntries.isEmpty())
        return false;

    int idx = (m_selectedIndex >= 0 && m_selectedIndex < m_playlistEntries.size())
              ? m_selectedIndex : 0;

    QString filename = m_playlistEntries[idx];

    // Resume from paused position if applicable.
    bool hasPauseInfo = (!m_pauseFile.isEmpty() && m_pauseFile == m_playlistEntries[idx]);

    QByteArray filenameBytes = filename.toLocal8Bit();
    if (!m_waveReader.Open(filenameBytes.data()))
        return false;

    if (hasPauseInfo) {
        m_waveReader.Seek(m_pausePos);
        m_playbackPositionBytes = m_pausePos;
    } else {
        m_playbackPositionBytes = 0;
    }

    // Bug fix: treat device-open failure as a hard error.
    if (!openOutDevice(m_waveReader.GetWaveFormat())) {
        m_waveReader.Close();
        return false;
    }

    if (resetDevice)
        waveOutReset(m_hWaveOut);

    setPlaying(true);
    return true;
}

void AppState::pause()
{
    // Bug fix: save to locals before stop() clears the members.
    // Bug fix: use GetFileName() (actual open file) not selectedIndex
    //   (UI selection), which could differ if user clicked another item.
    const bool inTail = m_suppressProgressUpdate;
    QString savedFile;
    long    savedPos = playbackPositionBytes();
    QString savedProgressText = m_progressText;
    if (m_waveReader.IsOpen()) {
        savedFile = QDir::fromNativeSeparators(
            QString::fromWCharArray(m_waveReader.GetFileName()));
    }

    stop();   // closes reader, clears m_pauseFile/m_pausePos, sets playing=false

    // Restore pause state that stop() cleared.
    m_pauseFile = savedFile;
    m_pausePos  = savedPos;
    // During a deferred gapless-advance tail, savedProgressText is the old track's
    // near-EOF time, but savedFile/savedPos are already for the new track at ~0.
    // stop() already set "0:00", which is correct — don't restore the stale text.
    if (!inTail && m_progressText != savedProgressText) {
        m_progressText = savedProgressText;
        emit progressTextChanged();
    }
}

void AppState::stop()
{
    m_waveReader.Close();

    if (m_hWaveOut)
        waveOutReset(m_hWaveOut);   // Returns all buffers; callback fills with silence.

    m_pauseFile.clear();
    m_pausePos = 0;
    m_playbackPositionBytes = 0;
    ++m_advanceGeneration;          // invalidate any pending deferred UI update
    const bool suppressedProgressUpdates = m_suppressProgressUpdate;
    m_suppressProgressUpdate = false;

    if (m_progressText != QStringLiteral("0:00")) {
        m_progressText = QStringLiteral("0:00");
        emit progressTextChanged();
    }

    setPlaying(false);

    // If stop() interrupted a deferred gapless advance, m_selectedIndex is already
    // on the new track but QML hasn't been notified. Emit now so the UI is consistent.
    if (suppressedProgressUpdates)
        emit selectedIndexChanged();
}

// ---------------------------------------------------------------------------
// Playlist
// ---------------------------------------------------------------------------
void AppState::loadStartupPlaylist()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
                         + QStringLiteral("/save_tracklist.txt");
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QStringList entries;
    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (!line.isEmpty())
            entries.append(line);
    }

    if (m_playlistEntries == entries)
        return;

    m_playlistEntries = entries;
    emit playlistEntriesChanged();

    if (!m_playlistEntries.isEmpty()) {
        m_selectedIndex = 0;
        emit selectedIndexChanged();
    }
}
