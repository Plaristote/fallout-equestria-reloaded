import QtQuick 2.15

ShaderEffect {
  id: root
  property variant source
  property color from: "transparent"
  property color to: "transparent"
  property color color: "transparent"
  property int   animState: 0
  property int   animDuration: 500
  property size  textureSize: Qt.size(width, height)

  width: source.width
  height: source.height

  vertexShader: "qrc:/game/hud/InteractionColorOverlay.shader.vert.qsb"
  fragmentShader: "qrc:/game/hud/InteractionColorOverlay.shader.frag.qsb"

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
