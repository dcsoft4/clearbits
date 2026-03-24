import QtQuick
import QtQuick.Controls

Screen01Form {
    id: form

    playPauseButton.text: appState.playing ? qsTr("Pause") : qsTr("Play")
    playPauseButton.onClicked: appState.togglePlaying()
    playlistView.model: appState.playlistEntries
    algoCombo.currentIndex: appState.algo
    progressText.text: appState.progressText

    // Sync view → C++: delegate clicks set currentIndex imperatively; forward to appState.
    playlistView.onCurrentIndexChanged: appState.selectedIndex = form.playlistView.currentIndex
    algoCombo.onCurrentIndexChanged: appState.algo = form.algoCombo.currentIndex

    Shortcut {
        sequence: "F1"
        onActivated: appState.algo = 0
    }

    Shortcut {
        sequence: "F2"
        onActivated: appState.algo = 1
    }

    Shortcut {
        sequence: "F3"
        onActivated: appState.algo = 2
    }

    Shortcut {
        sequence: "F4"
        onActivated: appState.algo = 3
    }

    // Sync C++ → view: use Connections (not a declarative binding) so that
    // imperative assignments from the delegate don't break the update path.
    Connections {
        target: appState
        function onSelectedIndexChanged() {
            form.playlistView.currentIndex = appState.selectedIndex
        }

        function onAlgoChanged() {
            form.algoCombo.currentIndex = appState.algo
        }
    }
}
