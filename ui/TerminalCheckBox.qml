import QtQuick 2.15
import QtQuick.Controls 2.15

CheckBox {
  property int size: 30;
  id: root
  height: size
  width:  size

  indicator: Rectangle {
    width: root.size
    height: root.size
    color: "transparent"
    border.width: 1
    border.color: "green"

    Text {
      anchors.centerIn: parent
      font.pixelSize: parent.height
      font.family: application.consoleFontName
      text: "x"
      visible: root.checked
      color: "green"
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.toggle()
    }
  }
  contentItem: Text {
    text: root.text
    font: root.font
    color: "green"
    verticalAlignment: Text.AlignVCenter
  }
}
