import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
  property color backgroundColor: down ? "green" : "transparent"

  id: button
  implicitHeight: 25
  font.family: application.consoleFont.name
  font.pointSize: application.consoleFont.normalSize
  contentItem: Text {
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
    text: button.text
    font: button.font
    color: "white"
  }
  background: Rectangle {
    border.color: "green"
    color: backgroundColor
    radius: 5
  }
}
