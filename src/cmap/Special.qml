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

  function getQualifierLabel(value) {
    if (value <= 0 || value > 10)
      return i18n.t("cmap.invalid");
    if (value > 8)
      return i18n.t("cmap.heroic");
    if (value > 6)
      return i18n.t("cmap.very-good");
    if (value > 4)
      return i18n.t("cmap.average");
    if (value > 2)
      return i18n.t("cmap.poor");
    return i18n.t("cmap.abysmal");
  }

  model: list
  delegate: RowLayout {
    Label {
      id: specialName
      Layout.minimumWidth: 100
      text: list[index]
      font.family: application.titleFontName
      font.pointSize: application.titleFont.pointSize
      font.letterSpacing: 3
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      background: UiStyle.Pane {}
      MouseArea { anchors.fill: parent; onClicked: selectProperty(propertyName[index]) }
    }
    Label {
      Layout.minimumWidth: 50
      text: characterSheet[propertyName [index]]
      font.family: application.consoleFontName
      font.pointSize: application.consoleFont.tinySize
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      padding: 10
      background: UiStyle.TerminalPane {}
      MouseArea { anchors.fill: parent; onClicked: selectProperty(propertyName[index]) }
    }
    Label {
      id: specialQualifier
      Layout.minimumWidth: 100
      text: getQualifierLabel(characterSheet[propertyName [index]])
      font.family: application.consoleFont.name
      font.pointSize: application.consoleFont.tinySize
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      padding: 10
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
