import QtQuick 2.15
import QtQuick.Effects

MultiEffect {
  property color color
  colorization: enabled
  colorizationColor: color
  anchors.fill: source
}
