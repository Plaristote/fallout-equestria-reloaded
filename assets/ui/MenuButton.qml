import QtQuick 2.15
import QtGraphicalEffects 1.15

Image {
  property bool pressed: parent.down
  source: "qrc:/assets/ui/menu-button.png"
  sourceClipRect: !pressed ? Qt.rect(0, 0, 248, 48) : Qt.rect(0, 49, 248, 48)
  fillMode: Image.Stretch

  ColorOverlay {
    anchors.fill: parent
    source: parent
    color: Qt.rgba(220, 220, 220, 0.4)
    visible: !parent.parent.enabled
  }
}
