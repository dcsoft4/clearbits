#pragma once

#include <QObject>
#include <QStringList>

class AppState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList playlistEntries READ playlistEntries NOTIFY playlistEntriesChanged)
    Q_PROPERTY(bool playing READ playing WRITE setPlaying NOTIFY playingChanged)

public:
    explicit AppState(QObject *parent = nullptr);

    QStringList playlistEntries() const;
    bool playing() const;

    Q_INVOKABLE void togglePlaying();

public slots:
    void setPlaying(bool playing);

signals:
    void playlistEntriesChanged();
    void playingChanged();

private:
    void loadPlaylist();

    QStringList m_playlistEntries;
    bool m_playing = false;
};
