import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

BorderImage {
  property string text: "default text"
  id: textDisplay
  source: "qrc:/assets/ui/dialog/text.png"
  border { top: 6; left: 287; right: 215; bottom: 11 }
  horizontalTileMode: BorderImage.Stretch
  verticalTileMode: BorderImage.Stretch
  height: 150

  Flickable {
    id: flickable
    clip: true
    contentHeight: dialogContent.height
    anchors.fill: parent
    anchors.leftMargin: 300
    anchors.rightMargin: 230
    anchors.topMargin: 13
    anchors.bottomMargin: 18

    ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

    Text {
      id: dialogContent
      color: "white"
      text: textDisplay.text
      textFormat: Text.StyledText
      wrapMode: Text.WordWrap
      horizontalAlignment: Qt.AlignJustify
      width: parent.width - 30
      font.family: application.consoleFontName
      font.pointSize: application.consoleFont.bigSize
    }
  }
}
