import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  property QtObject characterSheet
  property bool editable: false

  background: UiStyle.TerminalPane {}

  ColumnLayout {
    anchors.left: parent.left
    anchors.right: parent.right

    RowLayout {

      TerminalButton {
        visible: editable
        text: "+"
        onClicked: characterSheet.addExperience(characterSheet.xpNextLevel)
      }

      Text {
        Layout.alignment: Qt.AlignCenter
        text: i18n.t("cmap.level") + ": " + characterSheet.level
        color: "white"
        font.family: application.consoleFont.name
        font.pointSize: application.consoleFont.tinySize
      }

      TerminalButton {
        visible: editable
        text: "-"
        //onClicked: TODO
      }
    }

    Text {
      Layout.alignment: Qt.AlignCenter
      text: i18n.t("cmap.experience") + ": " + characterSheet.experience
      color: "white"
      font.family: application.consoleFont.name
      font.pointSize: application.consoleFont.tinySize
    }
    Text {
      Layout.alignment: Qt.AlignCenter
      text: i18n.t("cmap.next-level") + ": " + characterSheet.xpNextLevel
      color: "white"
      font.family: application.consoleFont.name
      font.pointSize: application.consoleFont.tinySize
    }
  }
}
