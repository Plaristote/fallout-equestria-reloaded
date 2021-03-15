import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject characterSheet
  property string selectedProperty
  property bool canEdit: false
  property var list: [
    "smallGuns", "bigGuns", "energyGuns",
    "explosives", "unarmed", "meleeWeapons",
    "lockpick", "sneak", "medicine", "repair",
    "science", "speech", "barter", "spellcasting",
    "steal", "gambling", "outdoorsman"
  ]

  background: UiStyle.TerminalPane {}

  signal selectProperty(string selectedName)

  Flickable {
    anchors.fill: parent
    anchors.margins: 5
    contentHeight: content.height
    contentWidth: content.width
    ScrollBar.vertical:   UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
    clip: true

    Column {
      id: content
      Repeater {
        model: root.list
        delegate: Row {
          property string propertyName: root.list[index]
          property color textColor: propertyName == root.selectedProperty ? "green" : "white"

          Text {
            text: i18n.t("cmap." + propertyName)
            color: textColor
            width: Math.max(150, root.width - (root.canEdit ? 175 : 100))
            topPadding: 5
            bottomPadding: 5
            font.family: application.consoleFontName
            font.pointSize: 8
            MouseArea { anchors.fill: parent; onClicked: root.selectProperty(propertyName) }
          }
          Text {
            text: characterSheet[root.list[index]] + "%"
            color: textColor
            width: 50
            topPadding: 5
            bottomPadding: 5
            font.family: application.consoleFontName
            font.pointSize: 8
            MouseArea { anchors.fill: parent; onClicked: root.selectProperty(propertyName) }
          }
          UiStyle.TerminalButton {
            visible: root.canEdit
            enabled: characterSheet.skillPoints > 0
            text: "+"
            onClicked: {
              const method = root.list[index] + "Increase";
              characterSheet[method]();
              root.selectProperty(propertyName);
            }
          }
          UiStyle.TerminalButton {
            visible: root.canEdit
            text: "-"
            onClicked: {
              const prefix      = root.list[index];
              const checkMethod = prefix + "CanDecrease";

              if (characterSheet[checkMethod]())
              {
                const method = prefix + "Decrease";
                characterSheet[method]();
              }
              root.selectProperty(propertyName);
            }
          }
        }
      }
    }
  }
}
