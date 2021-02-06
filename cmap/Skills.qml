import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject characterSheet
  property var list: [
    "smallGuns", "bigGuns", "energyGuns",
    "explosives", "unarmed", "meleeWeapons",
    "lockpick", "sneak", "medicine", "repair",
    "science", "speech", "barter", "spellcasting",
    "steal", "gambling", "outdoorsman"
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
      id: content
      Repeater {
        model: root.list
        delegate: Row {
          Text {
            text: qsTr(root.list[index])
            color: "white"
            width: Math.max(150, root.width - (root.canEdit() ? 150 : 50))
          }
          Text {
            text: characterSheet[root.list[index]] + "%"
            color: "white"
            width: 50
          }
          UiStyle.TerminalButton {
            visible: root.canEdit()
            enabled: characterSheet.skillPoints > 0
            text: "+"
            onClicked: {
              characterSheet[root.list[index]] += 1
              characterSheet.skillPoints -= 1
            }
          }
          UiStyle.TerminalButton {
            visible: root.canEdit()
            text: "-"
            onClicked: {
              characterSheet[root.list[index]] -= 1
              characterSheet.skillPoints += 1
            }
          }
        }
      }
    }
  }
}
