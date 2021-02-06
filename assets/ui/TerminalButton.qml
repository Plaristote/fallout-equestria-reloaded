import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
  id: button
  height: 15
  contentItem: Text {
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    text: button.text
    font: button.font
    color: "white"
  }
  background: Rectangle {
    border.color: "lightgreen"
    color: parent.down ? "lightgreen" : "transparent"
    radius: 5
  }
}
