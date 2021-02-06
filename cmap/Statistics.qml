import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property string selectedProperty
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

  signal selectProperty(string selectedName)

  ColumnLayout {
    Repeater {
      model: list
      delegate: Row {
        property string propertyName: list[index]
        property color textColor: selectedProperty == propertyName ? "green" : "white"
        Text {
          text: qsTr(propertyName)
          color: textColor
          Layout.alignment: Qt.AlignLeft
          width: 150
          MouseArea { anchors.fill: parent; onClicked: selectProperty(propertyName) }
        }
        Text {
          text: characterSheet[propertyName]
          color: textColor
          Layout.alignment: Qt.AlignRight
          MouseArea { anchors.fill: parent; onClicked: selectProperty(propertyName) }
        }
      }
    }
  }
}
