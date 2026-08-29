import QtQuick 2.15
import QtQuick.Controls 2.15

ScrollBar {
  id: scrollBar
  readonly property color barColor: "lightgreen"
  readonly property color activeColor: "white"

  background: Rectangle {
    border.color: scrollBar.active ? activeColor : barColor
    color: "transparent"
  }
  contentItem: Rectangle {
    color: scrollBar.active ? activeColor : barColor
    implicitWidth:  scrollBar.orientation === Qt.Horizontal ? 100 : 6
    implicitHeight: scrollBar.orientation === Qt.Horizontal ? 6 : 100
  }
}
