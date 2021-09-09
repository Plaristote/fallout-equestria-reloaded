import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../assets/ui" as UiStyle

RowLayout {
  property QtObject characterSheet
  property string label: i18n.t("cmap.available-skill-points")
  property int value: characterSheet.skillPoints

  Label {
    text: label
    font.family: application.titleFontName
    font.pointSize: 14
    color: "white"
    padding: 10
    leftPadding: 30
    background: UiStyle.Pane {}
    Layout.fillWidth: true
  }
  Label {
    text: value
    font.family: application.consoleFontName
    font.pointSize: 12
    color: "white"
    padding: 10
    background: UiStyle.TerminalPane {}
  }
}
