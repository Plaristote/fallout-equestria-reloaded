import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

RowLayout {
  Label {
    Layout.fillWidth: true
    text: i18n.t("cmap.available-points")
    font.family: application.titleFontName
    font.pointSize: 14
    color: "yellow"
    horizontalAlignment: Qt.AlignHCenter
    verticalAlignment: Qt.AlignVCenter
    background: UiStyle.Pane {}
  }
  Label {
    Layout.minimumWidth: 50
    text: characterSheet.specialPoints
    font.family: application.consoleFontName
    font.pointSize: 12
    color: "white"
    padding: 5
    horizontalAlignment: Qt.AlignHCenter
    verticalAlignment: Qt.AlignVCenter
    background: UiStyle.TerminalPane {}
  }
}
