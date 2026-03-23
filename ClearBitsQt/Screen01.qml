import QtQuick

Screen01Form {
    id: form

    playPauseButton.text: appState.playing ? qsTr("Pause") : qsTr("Play")
    playPauseButton.onClicked: appState.togglePlaying()
    playlistView.model: appState.playlistEntries

    // Sync view → C++: delegate clicks set currentIndex imperatively; forward to appState.
    playlistView.onCurrentIndexChanged: appState.selectedIndex = form.playlistView.currentIndex

    // Sync C++ → view: use Connections (not a declarative binding) so that
    // imperative assignments from the delegate don't break the update path.
    Connections {
        target: appState
        function onSelectedIndexChanged() {
            form.playlistView.currentIndex = appState.selectedIndex
        }
    }
}
