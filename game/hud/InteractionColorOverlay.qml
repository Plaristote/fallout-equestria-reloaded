import QtQuick 2.15
import QtGraphicalEffects 1.0

ColorOverlay {
  id: root
  property color from: "transparent"
  property color to: "transparent"
  property int   animState: 0
  property int   animDuration: 500

  color: from

  ColorAnimation on color {
    from: root.from
    to:   root.to
    running: root.visible && animState === 0
    duration: animDuration
    onFinished: animState = 1
  }

  ColorAnimation on color {
    from: root.to
    to:   root.from
    running: root.visible && animState === 1
    duration: animDuration
    onFinished: animState = 0
  }
}
