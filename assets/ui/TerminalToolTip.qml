import QtQuick 2.15
import QtQuick.Controls 2.15

ToolTip {
  property QtObject target
  property int minWidth: target.width
  visible: hovered && width > minWidth
  x: 0
  y: 0
  contentItem: Text {
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter
    text: target.text
    font: target.font
    color: target.textColor
  }
  background: Rectangle {
    property color autoColor: "transparent"
    border.color: target.enabled ? (target.activeFocus ? "white" : "green") : "gray"
    color: target.backgroundColor == autoColor ? "black" : target.backgroundColor
    radius: 5
  }
  MouseArea {
    anchors.fill: parent
    onClicked: target.clicked()
  }
}
