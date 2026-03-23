import QtQuick
import QtQuick.Controls
import ClearBitsDesign
import QtQuick.Layouts

Rectangle {
    width: Constants.width
    height: Constants.height

    color: palette.window

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.margins: 12
        spacing: 4

        // --- Header row: Logo | Title | Format ---
        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: 10
            spacing: 6

            Text {
                id: titleText
                text: qsTr("C L E A R B I T S   M U S I C   P L A Y E R")
                Layout.fillWidth: true
                font.family: Constants.font.family
                font.bold: true
                color: palette.windowText
            }

            Text {
                id: formatText
                text: qsTr("44 KHz / 16 bit / Stereo")
                font.family: Constants.font.family
                horizontalAlignment: Text.AlignRight
                color: palette.windowText
            }
        }

        // --- Controls row: Algorithm | Prev | Play | Next | Search ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            ComboBox {
                id: algoCombo
                model: ["Fixed", "VC", "CAPI", "RandomOrg"]
                Layout.preferredWidth: 120
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: prevButton
                text: qsTr("<< Back")
                Layout.preferredWidth: 100
            }

            Button {
                id: playPauseButton
                text: appState.playing ? qsTr("Pause") : qsTr("Play")
                Layout.preferredWidth: 100
                onClicked: appState.togglePlaying()
            }

            Button {
                id: nextButton
                text: qsTr("Next >>")
                Layout.preferredWidth: 100
            }

            Button {
                id: searchButton
                text: qsTr("Search...")
                Layout.preferredWidth: 100
                visible: false
            }
        }

        // --- Seek row: << < [progress] > >> ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: seekBackFastButton
                text: qsTr("<<")
                Layout.preferredWidth: 40
            }

            Button {
                id: seekBackSlowButton
                text: qsTr("<")
                Layout.preferredWidth: 40
            }

            Text {
                id: progressText
                text: qsTr("0:00")
                font.family: Constants.font.family
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 60
                color: palette.windowText
            }

            Button {
                id: seekFwdSlowButton
                text: qsTr(">")
                Layout.preferredWidth: 40
            }

            Button {
                id: seekFwdFastButton
                text: qsTr(">>")
                Layout.preferredWidth: 40
            }
        }

        // --- Playlist ---
        ListView {
            id: playlistView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 8
            clip: true
            model: appState.playlistEntries

            delegate: Text {
                width: playlistView.width
                text: modelData ?? ""
                font.family: Constants.font.family
                color: palette.windowText
                padding: 2
            }

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "#AAAAAA"
                border.width: 1
            }
        }

        // --- Footer row: Load | Clear | [spacer] | Shuffle ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Text {
                id: loadLink
                text: qsTr("Load")
                font.family: Constants.font.family
                color: palette.link
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor }
            }

            Text {
                id: clearLink
                text: qsTr("Clear")
                font.family: Constants.font.family
                color: palette.link
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor }
            }

            Text {
                id: deleteLink
                text: qsTr("Delete file")
                font.family: Constants.font.family
                color: palette.link
                visible: false
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor }
            }

            Text {
                id: moveLink
                text: qsTr("Move file")
                font.family: Constants.font.family
                color: palette.link
                visible: false
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor }
            }

            Item {
                Layout.fillWidth: true
            }

            CheckBox {
                id: shuffleCheck
                text: qsTr("Shuffle")
            }
        }
    }
}
