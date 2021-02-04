import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
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
    anchors {
      margins: 10
      top: characterNameRow.bottom
      bottom: controls.top
    }

    Pane {
      Layout.fillHeight: true
      Layout.fillWidth: true
      background: UiStyle.TerminalPane {}

      Flickable {
        anchors.fill: parent
        anchors.margins: 5
        contentHeight: zxy.height
        contentWidth: 200
        ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
        ScrollBar.horizontal: UiStyle.TerminalScrollbar { orientation: Qt.Horizontal }
        clip: true

        Column {
          id: zxy
          Repeater {
            model: 100
            Text {
              color: "white"
              text: index
            }
          }
        }
      }
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
