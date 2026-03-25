
/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: 640
    height: 480
    signal playlistItemDoubleClicked(int index)

    // Property aliases — Screen01.qml wires logic through these
    property alias playPauseButton: playPauseButton
    property alias prevButton: prevButton
    property alias nextButton: nextButton
    property alias algoCombo: algoCombo
    property alias progressText: progressText
    property alias shuffleCheck: shuffleCheck
    property alias playlistView: playlistView
    property alias loadLinkArea: loadLinkArea
    property alias clearLinkArea: clearLinkArea

    color: palette.window
    property alias seekBack2Button: seekBack2Button
    property alias seekBackButton: seekBackButton
    property alias seekForwardButton: seekForwardButton
    property alias seekForward2Button: seekForward2Button

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.margins: 12
        spacing: 4

        // --- Header row: Title | Format ---
        RowLayout {
            Layout.fillWidth: true
            Layout.bottomMargin: 10
            spacing: 6

            Text {
                id: titleText
                text: qsTr("C L E A R B I T S   M U S I C   P L A Y E R")
                Layout.fillWidth: true
                font.family: playPauseButton.font.family
                font.pointSize: playPauseButton.font.pointSize
                font.bold: true
                color: palette.windowText
            }
        }

        // --- Controls row: Algorithm | Prev | Play | Next ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            ComboBox {
                id: algoCombo
                model: ["[F1] Fixed", "[F2] rand()", "[F3]CAPI", "[F4] Random.org"]
                ToolTip.text: [
                    qsTr("Buffers are fixed size containing 2 seconds of audio, the standard method used by audio players"),
                    qsTr("Buffer sizes are determined by the VC rand() algorithm, notorious for poor randomness"),
                    qsTr("Buffer sizes are determined by the CryptGenRandom() algorithm, known for good randomness"),
                    qsTr("Buffer sizes are determined by real-world occurring random numbers from random.org")
                ][currentIndex]
                ToolTip.visible: hovered
                Layout.preferredWidth: 170
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: prevButton
                text: qsTr("<< Prev")
                Layout.preferredWidth: 100
                ToolTip { text: qsTr("[↑] Previous track"); visible: parent.hovered }
            }

            Button {
                id: playPauseButton
                text: qsTr("Play")
                Layout.preferredWidth: 100
                ToolTip { text: qsTr("[Space] Toggle play/pause"); visible: parent.hovered }
            }

            Button {
                id: nextButton
                text: qsTr("Next >>")
                Layout.preferredWidth: 100
                ToolTip { text: qsTr("[↓] Next track"); visible: parent.hovered }
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
                id: seekBack2Button
                text: qsTr("<<")
                Layout.preferredWidth: 50
                ToolTip { text: qsTr("[F5] Seek back 1 minute"); visible: parent.hovered; y: parent.height + 4 }
            }

            Button {
                id: seekBackButton
                text: qsTr("<")
                Layout.preferredWidth: 50
                ToolTip { text: qsTr("[F6] Seek back 10 seconds"); visible: parent.hovered; y: parent.height + 4 }
            }

            Text {
                id: progressText
                text: qsTr("0:00")
                font.family: playPauseButton.font.family
                font.pointSize: playPauseButton.font.pointSize
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 60
                color: palette.windowText
                HoverHandler {
                    id: progressHover
                }
                ToolTip { text: qsTr("Position in current track (M:SS)"); visible: progressHover.hovered; y: parent.height + 4 }
            }

            Button {
                id: seekForwardButton
                text: qsTr(">")
                Layout.preferredWidth: 50
                ToolTip { text: qsTr("[F7] Seek forward 10 seconds"); visible: parent.hovered; y: parent.height + 4 }
            }

            Button {
                id: seekForward2Button
                text: qsTr(">>")
                Layout.preferredWidth: 50
                ToolTip { text: qsTr("[F8] Seek forward 1 minute"); visible: parent.hovered; y: parent.height + 4 }
            }
        }

        // --- Playlist ---
        ListView {
            id: playlistView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 8
            clip: true
            HoverHandler {
                id: playlistHover
            }

            model: ListModel {
                id: playlistModel
            }

            ToolTip {
                text: qsTr("Click the Load link below to load audio tracks or playlists")
                visible: playlistHover.hovered && playlistView.count === 0
                x: playlistHover.point.position.x
                y: playlistHover.point.position.y + 4
            }


            highlight: Rectangle {
                color: palette.highlight
                opacity: 0.4
            }
            highlightMoveDuration: 0

            delegate: Item {
                width: playlistView.width
                height: delegateText.implicitHeight + 4

                HoverHandler {
                    id: delegateHover
                }

                Text {
                    id: delegateText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    text: modelData.replace(/\//g, "\\")
                    font.family: playPauseButton.font.family
                    font.pointSize: playPauseButton.font.pointSize
                    color: palette.windowText
                }

                ToolTip {
                    text: qsTr("Click to select this track; double-click to toggle play/pause")
                    visible: delegateHover.hovered
                    x: delegateHover.point.position.x
                    y: parent.height + 4
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: playlistView.currentIndex = index
                    onDoubleClicked: root.playlistItemDoubleClicked(index)
                }
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
                font.family: playPauseButton.font.family
                font.pointSize: playPauseButton.font.pointSize
                color: palette.link
                MouseArea {
                    id: loadLinkArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }

            Text {
                id: clearLink
                text: qsTr("Clear")
                font.family: playPauseButton.font.family
                font.pointSize: playPauseButton.font.pointSize
                color: palette.link
                MouseArea {
                    id: clearLinkArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }

            Text {
                id: deleteLink
                text: qsTr("Delete file")
                font.family: playPauseButton.font.family
                font.pointSize: playPauseButton.font.pointSize
                color: palette.link
                visible: false
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }

            Text {
                id: moveLink
                text: qsTr("Move file")
                font.family: playPauseButton.font.family
                font.pointSize: playPauseButton.font.pointSize
                color: palette.link
                visible: false
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }

            Item {
                Layout.fillWidth: true
            }

            CheckBox {
                id: shuffleCheck
                text: qsTr("Shuffle")
                visible: false
            }
        }
    }
}
