import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property var list: [
    "armor-class",
    "action-points",
    "carry-weight",
    "melee-damage",
    "damage-resistance",
    "poison-resistance",
    "radiation-resistance",
    "sequence",
    "healing-rate",
    "critical-chance"
  ]

  id: root
  background: UiStyle.TerminalPane {}

  ColumnLayout {
    Repeater {
      model: list
      delegate: Row {
        Text {
          text: qsTr(list[index])
          color: "white"
          Layout.alignment: Qt.AlignLeft
          width: 150
        }
        Text {
          text: "0"
          color: "white"
          Layout.alignment: Qt.AlignRight
        }
      }
    }
  }
}
