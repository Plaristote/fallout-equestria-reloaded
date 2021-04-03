import QtQuick 2.15
import QtGraphicalEffects 1.0

ColorOverlay {
  property color from
  property color to
  property int   animState: 0
  property int   animDuration: 500

  anchors.fill: source
  color: from

  ColorAnimation on color {
    from: parent.from
    to:   parent.to
    running: parent.visible && animState === 0
    duration: animDuration
    onFinished: animState = 1
  }

  ColorAnimation on color {
    from: parent.to
    to:   parent.from
    running: parent.visible && animState === 1
    duration: animDuration
    onFinished: animState = 0
  }
}
