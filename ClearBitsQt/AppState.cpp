#include "AppState.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFile>
#include <QIODevice>

#include <cstring>

namespace
{
const char kWaveDirectory[] = "C:/Temp/ClearbitsTracks";
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
    loadPlaylist();
    m_progressTimer.setInterval(500);
    connect(&m_progressTimer, &QTimer::timeout, this, &AppState::updateProgressText);

    m_waveReader.Create();
    CryptAcquireContext(&m_hProvider, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

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
}

void AppState::setAlgo(int algo)
{
    if (m_nAlgo == algo)
        return;

    if (m_hWaveOut) {
        if (m_playing) {
            m_playbackPositionBytes = playbackPositionBytes();
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
                // Reached end of file — close it; silence fills the remainder.
                m_waveReader.Close();
                setPlaying(false);
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

    return m_playbackPositionBytes + static_cast<long>(mmTime.u.cb);
}

void AppState::updateProgressText()
{
    if (!m_playing || m_wfx.nAvgBytesPerSec == 0)
        return;

    long bytes = playbackPositionBytes();
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

// ---------------------------------------------------------------------------
// Play / Pause / Stop
// ---------------------------------------------------------------------------
bool AppState::play()
{
    if (m_waveReader.IsOpen())
        return true;   // Already playing.

    if (m_playlistEntries.isEmpty())
        return false;

    int idx = (m_selectedIndex >= 0 && m_selectedIndex < m_playlistEntries.size())
              ? m_selectedIndex : 0;

    QString filename = QString::fromLatin1(kWaveDirectory) + "/" + m_playlistEntries[idx];

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

    waveOutReset(m_hWaveOut);

    setPlaying(true);
    return true;
}

void AppState::pause()
{
    // Bug fix: save to locals before stop() clears the members.
    // Bug fix: use GetFileName() (actual open file) not selectedIndex
    //   (UI selection), which could differ if user clicked another item.
    QString savedFile;
    long    savedPos = playbackPositionBytes();
    QString savedProgressText = m_progressText;
    if (m_waveReader.IsOpen()) {
        // GetFileName() returns the full path; store only the basename so it
        // matches the bare filenames in m_playlistEntries used by play().
        savedFile = QFileInfo(QString::fromWCharArray(m_waveReader.GetFileName())).fileName();
    }

    stop();   // closes reader, clears m_pauseFile/m_pausePos, sets playing=false

    // Restore pause state that stop() cleared.
    m_pauseFile = savedFile;
    m_pausePos  = savedPos;
    if (m_progressText != savedProgressText) {
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

    if (m_progressText != QStringLiteral("0:00")) {
        m_progressText = QStringLiteral("0:00");
        emit progressTextChanged();
    }

    setPlaying(false);
}

// ---------------------------------------------------------------------------
// Playlist
// ---------------------------------------------------------------------------
void AppState::loadPlaylist()
{
    QDir waveDirectory(QString::fromLatin1(kWaveDirectory));
    const QFileInfoList entries = waveDirectory.entryInfoList(
        QStringList() << "*.wav" << "*.mp3",
        QDir::Files | QDir::Readable,
        QDir::Name | QDir::IgnoreCase);

    QStringList playlistEntries;
    playlistEntries.reserve(entries.size());

    for (const QFileInfo &entry : entries)
        playlistEntries.append(entry.fileName());

    if (m_playlistEntries == playlistEntries)
        return;

    m_playlistEntries = playlistEntries;
    emit playlistEntriesChanged();
}
