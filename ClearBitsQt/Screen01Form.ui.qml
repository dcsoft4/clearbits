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

    // Property aliases — Screen01.qml wires logic through these
    property alias playPauseButton: playPauseButton
    property alias prevButton: prevButton
    property alias nextButton: nextButton
    property alias algoCombo: algoCombo
    property alias seekBackFastButton: seekBackFastButton
    property alias seekBackSlowButton: seekBackSlowButton
    property alias seekFwdSlowButton: seekFwdSlowButton
    property alias seekFwdFastButton: seekFwdFastButton
    property alias progressText: progressText
    property alias formatText: formatText
    property alias shuffleCheck: shuffleCheck
    property alias playlistView: playlistView
    property alias loadLinkArea: loadLinkArea
    property alias clearLinkArea: clearLinkArea

    color: palette.window

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
                font.family: Qt.application.font.family
                font.bold: true
                color: palette.windowText
            }

            Text {
                id: formatText
                text: qsTr("44 KHz / 16 bit / Stereo")
                font.family: Qt.application.font.family
                horizontalAlignment: Text.AlignRight
                color: palette.windowText
            }
        }

        // --- Controls row: Algorithm | Prev | Play | Next ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            ComboBox {
                id: algoCombo
                model: ["Fixed", "VC", "CAPI", "Random.org"]
                Layout.preferredWidth: 120
            }

            Item { Layout.fillWidth: true }

            Button {
                id: prevButton
                text: qsTr("<< Back")
                Layout.preferredWidth: 100
            }

            Button {
                id: playPauseButton
                text: qsTr("Play")
                Layout.preferredWidth: 100
            }

            Button {
                id: nextButton
                text: qsTr("Next >>")
                Layout.preferredWidth: 100
            }
        }

        // --- Seek row: << < [progress] > >> ---
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Item { Layout.fillWidth: true }

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
                font.family: Qt.application.font.family
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

            model: ListModel { id: playlistModel }

            highlight: Rectangle {
                color: palette.highlight
                opacity: 0.4
            }
            highlightMoveDuration: 0

            delegate: Item {
                width: playlistView.width
                height: delegateText.implicitHeight + 4

                Text {
                    id: delegateText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                    text: modelData
                    font.family: Qt.application.font.family
                    color: palette.windowText
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: playlistView.currentIndex = index
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
                font.family: Qt.application.font.family
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
                font.family: Qt.application.font.family
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
                font.family: Qt.application.font.family
                color: palette.link
                visible: false
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor }
            }

            Text {
                id: moveLink
                text: qsTr("Move file")
                font.family: Qt.application.font.family
                color: palette.link
                visible: false
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor }
            }

            Item { Layout.fillWidth: true }

            CheckBox {
                id: shuffleCheck
                text: qsTr("Shuffle")
            }
        }
    }
}
