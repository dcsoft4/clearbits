import QtQuick

Screen01Form {
    playPauseButton.text: appState.playing ? qsTr("Pause") : qsTr("Play")
    playPauseButton.onClicked: appState.togglePlaying()
    playlistView.model: appState.playlistEntries
}
