import QtQuick 2.15
import QtQuick.Effects

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

  layer.enabled: !parent.enabled
  layer.effect: MultiEffect {
    saturation: -1
  }
}
