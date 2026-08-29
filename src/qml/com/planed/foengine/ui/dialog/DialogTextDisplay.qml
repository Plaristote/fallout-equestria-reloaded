import QtQuick 2.15
import QtQuick.Controls 2.15
import "../style" as UiStyle

BorderImage {
  property QtObject customFont
  property string text: "default text"
  readonly property bool textConcealed: gameManager.currentGame.fastPassTime
  id: textDisplay
  source: "qrc:/assets/ui/dialog/text.png"
  border { top: 6; left: 287; right: 215; bottom: 11 }
  horizontalTileMode: BorderImage.Stretch
  verticalTileMode: BorderImage.Stretch
  height: 150

  FontLoader {
    id: customFontLoader
    source: customFont ? "file://" + customFont.uri : i18n.consoleFont
  }

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
      text: !textDisplay.textConcealed ? textDisplay.text : ""
      textFormat: Text.StyledText
      wrapMode: Text.WordWrap
      horizontalAlignment: Qt.AlignJustify
      width: parent.width - 30
      font.family:    customFont?.enabled ? customFontLoader.name : application.consoleFontName
      font.pointSize: customFont?.enabled ? customFont.pixelSize  : application.consoleFont.bigSize
      onTextChanged: flickable.contentY = 0
    }
  }
}
