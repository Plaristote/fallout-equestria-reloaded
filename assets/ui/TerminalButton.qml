import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
  property color backgroundColor: down ? "green" : "transparent"

  id: button
  height: 15
  font.family: application.consoleFontName
  contentItem: Text {
    horizontalAlignment: Text.AlignHCenter
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
