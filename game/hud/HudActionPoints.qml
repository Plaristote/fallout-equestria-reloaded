import QtQuick 2.15
import QtGraphicalEffects 1.0

Item {
  property int maxActionPoints
  property int actionPoints
  id: actionPointPane
  height: 15

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
        sourceClipRect: Qt.rect(available ? 404 : 374, 92, 30, 30)
        height: 15
        width:  15
      }
    }
  }

  ColorOverlay {
    source: parent
    anchors.fill: parent
    color: "black"
    visible: !levelController.combat
  }
}
