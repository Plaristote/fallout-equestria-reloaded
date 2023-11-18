import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Button {
  property color textColor: "white"

  implicitWidth: 248
  implicitHeight: 48
  font.bold: true
  font.family: application.titleFontName
  font.pointSize: application.titleFont.tinySize
  font.letterSpacing: 3
  contentItem: Text {
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    leftPadding: 25
    text: parent.text
    font: parent.font
    color: textColor
    styleColor: "gray"
    style: {
      if (parent.focus)
        return Text.Outline;
      else if (parent.down)
        return Text.Sunken;
      return Text.Normal;
    }
  }
  background: UiStyle.MenuButton {}
}
