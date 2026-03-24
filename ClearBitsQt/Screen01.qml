import QtQuick
import QtQuick.Controls

Screen01Form {
    id: form

    function seekBack2() { appState.seekRelativeSeconds(-60) }
    function seekBack() { appState.seekRelativeSeconds(-10) }
    function seekForward() { appState.seekRelativeSeconds(10) }
    function seekForward2() { appState.seekRelativeSeconds(60) }

    playPauseButton.text: appState.playing ? qsTr("Pause") : qsTr("Play")
    playPauseButton.onClicked: appState.togglePlaying()
    prevButton.onClicked: appState.prevTrack()
    nextButton.onClicked: appState.nextTrack()
    seekBack2Button.onClicked: seekBack2()
    seekBackButton.onClicked: seekBack()
    seekForwardButton.onClicked: seekForward()
    seekForward2Button.onClicked: seekForward2()
    loadLinkArea.onClicked: appState.loadPlaylist()
    clearLinkArea.onClicked: appState.clearPlaylist()
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

    Shortcut {
        sequence: "F5"
        onActivated: seekBack2()
    }

    Shortcut {
        sequence: "F6"
        onActivated: seekBack()
    }

    Shortcut {
        sequence: "F7"
        onActivated: seekForward()
    }

    Shortcut {
        sequence: "F8"
        onActivated: seekForward2()
    }

    Shortcut {
        sequence: "Up"
        onActivated: appState.prevTrack()
    }

    Shortcut {
        sequence: "Down"
        onActivated: appState.nextTrack()
    }

    Shortcut {
        sequence: "Space"
        onActivated: appState.togglePlaying()
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
