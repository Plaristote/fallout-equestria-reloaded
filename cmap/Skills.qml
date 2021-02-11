import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject characterSheet
  property string selectedProperty
  property var list: [
    "smallGuns", "bigGuns", "energyGuns",
    "explosives", "unarmed", "meleeWeapons",
    "lockpick", "sneak", "medicine", "repair",
    "science", "speech", "barter", "spellcasting",
    "steal", "gambling", "outdoorsman"
  ]

  background: UiStyle.TerminalPane {}

  signal selectProperty(string selectedName)

  function canEdit() {
    return true;
  }

  Flickable {
    anchors.fill: parent
    anchors.margins: 5
    contentHeight: content.height
    contentWidth: content.width
    ScrollBar.vertical:   UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
    clip: true

    Column {
      id: content
      spacing: 10
      Repeater {
        model: root.list
        delegate: Row {
          property string propertyName: root.list[index]
          property color textColor: propertyName == root.selectedProperty ? "green" : "white"


          Text {
            text: qsTr(propertyName)
            color: textColor
            width: Math.max(150, root.width - (root.canEdit() ? 150 : 50))
            MouseArea { anchors.fill: parent; onClicked: root.selectProperty(propertyName) }
          }
          Text {
            text: characterSheet[root.list[index]] + "%"
            color: textColor
            width: 50
            MouseArea { anchors.fill: parent; onClicked: root.selectProperty(propertyName) }
          }
          UiStyle.TerminalButton {
            visible: root.canEdit()
            enabled: characterSheet.skillPoints > 0
            text: "+"
            onClicked: {
              characterSheet[root.list[index]] += 1;
              characterSheet.skillPoints -= 1;
              root.selectProperty(propertyName);
            }
          }
          UiStyle.TerminalButton {
            visible: root.canEdit()
            text: "-"
            onClicked: {
              characterSheet[root.list[index]] -= 1;
              characterSheet.skillPoints += 1;
              root.selectProperty(propertyName);
            }
          }
        }
      }
    }
  }
}
