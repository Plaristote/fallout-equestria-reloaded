import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Repeater {
  readonly property var list: [
    "STR", "PER", "END", "CHA", "INT", "AGI", "LUC"
  ]
  readonly property var propertyName: [
    "strength", "perception", "endurance", "charisma", "intelligence", "agility", "luck"
  ]
  property QtObject characterSheet
  property bool editable: false

  signal selectProperty(string selectedName);

  model: list
  delegate: RowLayout {
    Label {
      id: specialName
      Layout.minimumWidth: 100
      text: list[index]
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      background: UiStyle.Pane {}
      MouseArea { anchors.fill: parent; onClicked: selectProperty(propertyName[index]) }
    }
    Label {
      Layout.minimumWidth: 50
      text: characterSheet[propertyName [index]]
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      padding: 5
      background: UiStyle.TerminalPane {}
      MouseArea { anchors.fill: parent; onClicked: selectProperty(propertyName[index]) }
    }
    Label {
      id: specialQualifier
      Layout.minimumWidth: 100
      text: "Average"
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      padding: 5
      background: UiStyle.TerminalPane {}
      MouseArea { anchors.fill: parent; onClicked: selectProperty(propertyName[index]) }
    }
    UiStyle.TinyButton {
      visible: editable
      text: "+"
      enabled: characterSheet.specialPoints > 0
      onClicked: {
        const value = characterSheet[propertyName[index]];
        if (value < 10) {
          characterSheet[propertyName[index]] += 1
          characterSheet.specialPoints -= 1;
        }
        selectProperty(propertyName[index])
      }
    }
    UiStyle.TinyButton {
      visible: editable
      text: "-"
      onClicked: {
        const value = characterSheet[propertyName[index]];
        if (value > 1) {
          characterSheet[propertyName[index]] -= 1
          characterSheet.specialPoints += 1;
        }
        selectProperty(propertyName[index])
      }
    }
  }
}
