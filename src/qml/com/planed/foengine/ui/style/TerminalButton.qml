import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
  property color backgroundColor: down ? "green" : "transparent"
  property color textColor: "white"

  id: button
  implicitHeight: 25
  font.family: application.consoleFont.name
  font.pointSize: application.consoleFont.normalSize
  contentItem: Text {
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
    text: button.text
    font: button.font
    color: textColor
  }
  background: Rectangle {
    border.color: button.enabled ? (button.activeFocus ? "white" : "green") : "gray"
    color: backgroundColor
    radius: 5
  }
  Connections {
    target: button
    function onDownChanged() {
      if (typeof soundManager != "undefined")
        soundManager.play(button.down ? "ui/term-btn-in" : "ui/term-btn-out");
    }
  }
}
