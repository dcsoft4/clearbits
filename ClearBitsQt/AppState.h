#pragma once

#include <QObject>
#include <QStringList>
#include <QEvent>

#include "StdAfx.h"

#include "SAMPBUF.h"
#include "MCWaveReader.h"

#define NUM_SAMPLE_BUFFERS 2
#define MAX_SOUNDBUF_SIZE  (176400 * 5)   // ~5 seconds at 44.1K/16-bit/stereo

// Custom Qt event posted from the waveOut callback thread when a buffer finishes.
class WomDoneEvent : public QEvent
{
public:
    static const QEvent::Type Type = static_cast<QEvent::Type>(QEvent::User + 1);
    explicit WomDoneEvent(CSampleBuffer *sb) : QEvent(Type), m_sb(sb) {}
    CSampleBuffer *sampleBuffer() const { return m_sb; }
private:
    CSampleBuffer *m_sb;
};


class AppState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList playlistEntries READ playlistEntries NOTIFY playlistEntriesChanged)
    Q_PROPERTY(bool playing READ playing WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)

public:
    explicit AppState(QObject *parent = nullptr);
    ~AppState() override;

    QStringList playlistEntries() const;
    bool playing() const;
    int selectedIndex() const;

    Q_INVOKABLE void togglePlaying();

public slots:
    void setPlaying(bool playing);
    void setSelectedIndex(int index);

signals:
    void playlistEntriesChanged();
    void playingChanged();
    void selectedIndexChanged();

protected:
    bool event(QEvent *e) override;

private:
    void loadPlaylist();
    bool openOutDevice(WAVEFORMATEX *pwfx);   // nullptr => standard 44.1K/16-bit/stereo
    void closeOutDevice();
    void playBuffer(CSampleBuffer *pSB);
    bool play();
    void pause();
    void stop();
    LONG getNextBufSize_Fixed();

    static void CALLBACK waveOutCallback(HWAVEOUT hwo, UINT uMsg,
                                         DWORD_PTR dwInstance,
                                         DWORD_PTR dwParam1,
                                         DWORD_PTR dwParam2);

    QStringList m_playlistEntries;
    bool m_playing = false;
    int m_selectedIndex = -1;

    HWAVEOUT m_hWaveOut = nullptr;
    WAVEFORMATEX m_wfx = {};
    CSampleBuffer m_arrSB[NUM_SAMPLE_BUFFERS];

    McWaveReader m_waveReader;

    QString m_pauseFile;
    long    m_pausePos = 0;
};
