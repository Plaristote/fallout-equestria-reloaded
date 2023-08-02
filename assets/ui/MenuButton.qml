import QtQuick 2.15

BorderImage {
  property bool pressed: parent.down
  property bool active: parent.activeFocus
  property string mode: pressed ? "pressed" : (active ? "active" : "normal")
  source: `qrc:/assets/ui/menuButton/${mode}.png`

  border {
    top: 8
    bottom: 8
    left: 51
    right: 22
  }

  horizontalTileMode: BorderImage.Repeat
  verticalTileMode: BorderImage.Repeat
/*
  ColorOverlay {
    anchors.fill: parent
    source: parent
    color: Qt.rgba(220, 220, 220, 0.4)
    visible: !parent.parent.enabled
  }
*/
}
