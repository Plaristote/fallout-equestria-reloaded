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

    Text {
      Layout.alignment: Qt.AlignCenter
      text: i18n.t("cmap.level") + ": " + characterSheet.level
      color: "white"
      font.family: application.consoleFontName
      font.pointSize: 8

      TerminalButton {
        visible: editable
        anchors.left: parent.right; anchors.top: parent.top
        anchors.leftMargin: 10
        text: "+"
        onClicked: characterSheet.addExperience(characterSheet.xpNextLevel)
      }

      TerminalButton {
        visible: editable
        anchors.right: parent.left; anchors.top: parent.top
        anchors.rightMargin: 10
        text: "-"
        //onClicked: TODO
      }
    }
    Text {
      Layout.alignment: Qt.AlignCenter
      text: i18n.t("cmap.experience") + ": " + characterSheet.experience
      color: "white"
      font.family: application.consoleFontName
      font.pointSize: 8
    }
    Text {
      Layout.alignment: Qt.AlignCenter
      text: i18n.t("cmap.next-level") + ": " + characterSheet.xpNextLevel
      color: "white"
      font.family: application.consoleFontName
      font.pointSize: 8
    }
  }
}
