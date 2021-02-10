import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
  id: root
  property string name
  property bool   isNewSlot: false
  property bool   selected: false
  property color  currentColor: selected ? "white" : "lightgreen"

  contentItem: Text {
    text: root.name
    color: root.currentColor
  }

  background: Rectangle {
    color: "transparent"
    radius: 5
    border.color: root.currentColor
    border.width: 1
  }
}
