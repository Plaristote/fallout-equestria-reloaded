import QtQuick 2.15

Item {
  property int maxActionPoints
  property int actionPoints
  id: actionPointPane
  height: 15
  visible: levelController.combat

  Row {
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 3

    Repeater {
      model: maxActionPoints
      delegate: Image {
        property bool available: index < actionPoints
        source: "qrc:/assets/ui/hud.png"
        sourceClipRect: Qt.rect(available ? 0 : 59, 246, 59, 59)
        height: 15
        width:  15
        PropertyAnimation on scale {
          from: 1.2
          to: 1
          duration: 1000
          loops: Animation.Infinite
          running: available
          alwaysRunToEnd: true
        }
      }
    }
  }
}
