import QtQuick 2.15

ShaderEffect {
  id: root
  property variant source: triangleImage
  property color from: "transparent"
  property color to: "transparent"
  property color color: "transparent"
  property int   animState: 0
  property int   animDuration: 500

  width: source.width
  height: source.height

  vertexShader: "
      uniform highp mat4 qt_Matrix;
      attribute highp vec4 qt_Vertex;
      attribute highp vec2 qt_MultiTexCoord0;
      varying highp vec2 coord;
      void main() {
          coord = qt_MultiTexCoord0;
          gl_Position = qt_Matrix * qt_Vertex;
      }"
  fragmentShader: "
      varying highp vec2 coord;
      uniform sampler2D source;
      uniform vec4 color;
      uniform lowp float qt_Opacity;
      void main() {
          lowp vec4 tex = texture2D(source, coord);
          if (tex.a != 0.0) {
            gl_FragColor = color * qt_Opacity;
          }
          else {
            gl_FragColor = tex * qt_Opacity;
          }
      }"

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
