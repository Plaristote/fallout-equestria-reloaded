import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Button {
  id: root
  property color textColor: "white"

  implicitWidth: textMetrics.tightBoundingRect.width + contentItem.leftPadding + 25
  implicitHeight: 48
  font.bold: true
  font.family: application.titleFontName
  font.pointSize: application.titleFont.tinySize
  font.letterSpacing: 3

  TextMetrics {
    id: textMetrics
    font: root.font
    text: root.text
  }

  contentItem: Text {
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
    leftPadding: 50
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
