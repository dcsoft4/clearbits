#include "AppState.h"

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

namespace
{
const char kWaveDirectory[] = "C:/Temp/ClearbitsTracks";
}

AppState::AppState(QObject *parent)
    : QObject(parent)
{
    loadPlaylist();
}

QStringList AppState::playlistEntries() const
{
    return m_playlistEntries;
}

bool AppState::playing() const
{
    return m_playing;
}

void AppState::togglePlaying()
{
    setPlaying(!m_playing);
}

void AppState::setPlaying(bool playing)
{
    if (m_playing == playing) {
        return;
    }

    m_playing = playing;
    emit playingChanged();
}

void AppState::loadPlaylist()
{
    QDir waveDirectory(QString::fromLatin1(kWaveDirectory));
    const QFileInfoList entries = waveDirectory.entryInfoList(
        QStringList() << "*.wav" << "*.mp3",
        QDir::Files | QDir::Readable,
        QDir::Name | QDir::IgnoreCase);

    QStringList playlistEntries;
    playlistEntries.reserve(entries.size());

    for (const QFileInfo &entry : entries) {
        playlistEntries.append(entry.fileName());
    }

    if (m_playlistEntries == playlistEntries) {
        return;
    }

    m_playlistEntries = playlistEntries;
    emit playlistEntriesChanged();
}
