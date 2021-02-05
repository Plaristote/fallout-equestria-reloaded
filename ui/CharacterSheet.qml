import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "qrc:/cmap" as CMAP

Pane {
  id: root
  property QtObject characterSheet
  property var mode: "create"
  readonly property bool createMode: mode === "create"

  background: UiStyle.Pane {}

  RowLayout {
    id: characterNameRow
    spacing: 10
    height: 50
    anchors {
      margins: 10
      top: parent.top
      left: parent.left
    }

    TextField {
      id: characterName
      Layout.minimumWidth: 200
      Layout.fillHeight: true
      readOnly: !createMode
      color: "white"
      placeholderText: qsTr("Your name")
      placeholderTextColor: "lightgray"
      background: UiStyle.Label {
        style: parent.focus ? "dark" : "base"
      }
    }

    TextField {
      id: characterAge
      Layout.minimumWidth: 50
      Layout.fillHeight: true
      color: "white"
      readOnly: !createMode
      placeholderText: qsTr("age")
      placeholderTextColor: "lightgray"
      validator: IntValidator { bottom: 15; top: 100 }
      background: UiStyle.Label {
        style: parent.focus ? "dark" : "base"
      }
    }

    ComboBox {
      id: characterGender
      Layout.minimumWidth: 200
      Layout.fillHeight: true
      enabled: createMode
      textRole: "text"
      valueRole: "value"
      contentItem: Text {
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: characterGender.currentText
        color: "white"
      }
      background: UiStyle.Label {
        style: parent.focus ? "dark" : "base"
      }
      model: [
        { value: 0, text: qsTr("Male") },
        { value: 1, text: qsTr("Female") }
      ]
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
      visible: true // TODO only when skill pointz are available
      Label {
        text: "Available skill points"
        color: "white"
        padding: 10
        leftPadding: 30
        background: UiStyle.Pane {}
        Layout.fillWidth: true
      }
      Label {
        text: "0"
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
