import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../cmap" as CMAP

Pane {
  id: root
  property QtObject characterSheet
  property var mode: "create"
  readonly property bool createMode: mode === "create"
  readonly property bool editMode: mode === "edit"

  background: UiStyle.Pane {}

  CMAP.PersonalInfo {
    id: characterNameRow
    editable: createMode
    anchors {
      margins: 10
      top: parent.top
      left: parent.left
    }
  }

  RowLayout {
    id: statisticsRow
    spacing: 10

    anchors {
      margins: 10
      top: characterNameRow.bottom
    }

    ColumnLayout {
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignTop

      CMAP.Special {
        editable: createMode
        characterSheet: root.characterSheet
      }

      RowLayout {
        visible: createMode
        Layout.topMargin: 15
        Label {
          Layout.fillWidth: true
          text: qsTr("Available points")
          color: "yellow"
          horizontalAlignment: Qt.AlignHCenter
          verticalAlignment: Qt.AlignVCenter
          background: UiStyle.Pane {}
        }
        Label {
          Layout.minimumWidth: 50
          text: characterSheet.specialPoints
          color: "white"
          padding: 5
          horizontalAlignment: Qt.AlignHCenter
          verticalAlignment: Qt.AlignVCenter
          background: UiStyle.TerminalPane {}
        }
      }

      CMAP.Experience {
        characterSheet: root.characterSheet
        Layout.fillWidth: true
        Layout.topMargin: 15
      }
    }

    ColumnLayout {
      Layout.alignment: Qt.AlignTop

      CMAP.Status {
        characterSheet: root.characterSheet
        Layout.fillWidth: true
      }

      CMAP.Statistics {
        characterSheet: root.characterSheet
        Layout.fillWidth: true
      }
    }
  }

  ColumnLayout {
    id: thirdColumn
    anchors { left: statisticsRow.right; top: statisticsRow.top; bottom: controls.top; right: parent.right }
    anchors.leftMargin: 10

    CMAP.Skills {
      characterSheet: root.characterSheet
      Layout.alignment: Qt.AlignTop
      Layout.fillWidth: true
      Layout.fillHeight: true
    }

    RowLayout {
      visible: editMode || createMode
      Label {
        text: "Available skill points"
        color: "white"
        padding: 10
        leftPadding: 30
        background: UiStyle.Pane {}
        Layout.fillWidth: true
      }
      Label {
        text: characterSheet.skillPoints
        color: "white"
        padding: 10
        background: UiStyle.TerminalPane {}
      }
    }

    Pane {
      id: descriptionPanel
      Layout.fillWidth: true
      Layout.minimumHeight: 200
      background: UiStyle.PaperPane {}

      Text {
        id: descriptionTitle
        text: qsTr("Cutie Mark Acquisition Program")
        font.bold: true
        font.pointSize: 15
        wrapMode: Text.WordWrap
        width: parent.width
      }

      Image {
        id: descriptionPicture
      }

      Flickable {
        clip: true
        contentHeight: descriptionContent.height

        anchors {
          top: descriptionTitle.bottom
          left: descriptionPicture.right
          right: parent.right
          bottom: parent.bottom
        }

        Text {
          id: descriptionContent
          text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In risus tellus, commodo vitae pretium at, viverra elementum velit. Phasellus et finibus magna. Praesent fermentum, tortor eu vehicula hendrerit, magna sem iaculis metus, vitae placerat quam ligula vitae nibh. In accumsan sodales tortor, nec efficitur tortor bibendum in. In hac habitasse platea dictumst. Vivamus pellentesque ex sit amet mauris fringilla porta. Phasellus mollis enim nec lacus tempus, sit amet pretium nulla vestibulum. Suspendisse non tortor quis tortor auctor posuere nec a ligula. Suspendisse a lectus sollicitudin, feugiat dolor consectetur, tempus ligula. Etiam at diam at quam rutrum fringilla. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam facilisis sodales eros eget viverra."
          wrapMode: Text.WordWrap
          width: parent.width
        }
      }
    }
  }

  Item {
    id: tabView
    anchors {
      top: statisticsRow.bottom
      left: statisticsRow.left
      right: statisticsRow.right
      bottom: controls.top
      topMargin: 10
    }

    property var tabs: ["perks", "traits", "reputation", "kills"]
    property string currentTab: "perks"

    Row {
      id: tabList
      Repeater {
        model: tabView.tabs.length
        delegate: Button {
          text: tabView.tabs[index]
          font.bold: tabView.currentTab === tabView.tabs[index]
          background: UiStyle.Tab {}
          contentItem: Text {
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: parent.text
            font: parent.font
            color: "white"
          }
          onClicked: tabView.currentTab = tabView.tabs[index]
        }
      }
    }

    Pane {
      id: perksPane
      anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
      visible: tabView.currentTab === "perks"
      background: UiStyle.TerminalPane {}
    }
  }

  RowLayout {
    id: controls
    anchors { bottom: parent.bottom; right: parent.right }
    MenuButton {
      text: qsTr("Confirm")
      enabled: false
      onClicked: console.log("accepted")
    }
    MenuButton {
      text: qsTr("Cancel")
      onClicked: application.popView()
    }
  }
}
