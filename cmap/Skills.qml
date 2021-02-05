import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject characterSheet
  property var list: [
    "small-guns", "big-guns", "energy-guns",
    "explosives", "unarmed", "melee-weapons",
    "lockpick", "sneak", "medecine", "repair",
    "science", "speak", "trade", "spellcasting",
    "steal", "barter", "outdoorspony"
  ]

  background: UiStyle.TerminalPane {}

  function canEdit() {
    return true;
  }

  Flickable {
    anchors.fill: parent
    anchors.margins: 5
    contentHeight: content.height
    contentWidth: content.width
    ScrollBar.vertical:   UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
    ScrollBar.horizontal: UiStyle.TerminalScrollbar { orientation: Qt.Horizontal }
    clip: true

    Column {
      x: width < root.width ? root.width / 2 - width / 2 - 10 : 0

      id: content
      Repeater {
        model: root.list
        delegate: Row {
          Text {
            text: qsTr(root.list[index])
            color: "white"
            width: 250
          }
          Text {
            text: "0%"
            color: "white"
            width: 50
          }
          UiStyle.TerminalButton {
            visible: root.canEdit()
            text: "+"
          }
          UiStyle.TerminalButton {
            visible: root.canEdit()
            text: "-"
          }
        }
      }
    }
  }
}
