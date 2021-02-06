import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property var list: [
    "armorClass",
    "actionPoints",
    "carryWeight",
    "meleeDamage",
    "damageResistance",
    "poisonResistance",
    "radiationResistance",
    "sequence",
    "healingRate",
    "criticalChance"
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
          text: characterSheet[list[index]]
          color: "white"
          Layout.alignment: Qt.AlignRight
        }
      }
    }
  }
}
