import QtQuick 2.15

ShaderEffect {
  property variant source
  property color color

  visible: enabled
  anchors.fill: parent
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
          lowp float withAmbientColor = tex.a != 0.0 ? 1.0 : 0.0;
          gl_FragColor = vec4(
            tex.r + withAmbientColor * (color.r + tex.r + tex.r) * 0.3,
            tex.g + withAmbientColor * (color.g + tex.g + tex.g) * 0.3,
            tex.b + withAmbientColor * (color.b + tex.b + tex.b) * 0.3,
            tex.a
          ) * qt_Opacity;
      }"
}
