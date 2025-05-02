import QtQuick 2.15

Image {
  property string label
  source: "qrc:/assets/ui/hud.png"
  sourceClipRect: Qt.rect(204, 0, 232, 32)
  height: 32
  width: 232

  Text {
    anchors.centerIn: parent
    text: label
    color: "yellow"
    font.family: application.consoleFontName
    font.pointSize: application.consoleFont.tinySize
    font.capitalization: Font.Capitalize
    font.bold: true
  }
}
