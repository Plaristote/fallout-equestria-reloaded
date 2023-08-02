import QtQuick 2.15

ShaderEffect {
  property variant source
  property color color

  visible: enabled
  anchors.fill: parent
  vertexShader: "qrc:/game/level/Daylight.vert.qsb"
  fragmentShader: "qrc:/game/level/Daylight.frag.qsb"
}
