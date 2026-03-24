import QtQuick
import QtQuick.Controls

Screen01Form {
    id: form

    function seekBackLarge() { appState.seekRelativeSeconds(-60) }
    function seekBackSmall() { appState.seekRelativeSeconds(-10) }
    function seekForwardSmall() { appState.seekRelativeSeconds(10) }
    function seekForwardLarge() { appState.seekRelativeSeconds(60) }

    playPauseButton.text: appState.playing ? qsTr("Pause") : qsTr("Play")
    playPauseButton.onClicked: appState.togglePlaying()
    prevButton.onClicked: appState.prevTrack()
    nextButton.onClicked: appState.nextTrack()
    seekBackFastButton.onClicked: seekBackLarge()
    seekBackButton.onClicked: seekBackSmall()
    seekForwardButton.onClicked: seekForwardSmall()
    seekForwardLargeButton.onClicked: seekForwardLarge()
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
        onActivated: seekBackLarge()
    }

    Shortcut {
        sequence: "F6"
        onActivated: seekBackSmall()
    }

    Shortcut {
        sequence: "F7"
        onActivated: seekForwardSmall()
    }

    Shortcut {
        sequence: "F8"
        onActivated: seekForwardLarge()
    }

    Shortcut {
        sequence: "Up"
        onActivated: appState.prevTrack()
    }

    Shortcut {
        sequence: "Down"
        onActivated: appState.nextTrack()
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
