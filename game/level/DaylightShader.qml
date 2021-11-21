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
          if (tex.a != 0.0) {
            lowp float r = color.r + tex.r + tex.r;
            lowp float g = color.g + tex.g + tex.g;
            lowp float b = color.b + tex.b + tex.b;
            gl_FragColor = vec4(r * 0.3, g * 0.3, b * 0.3, tex.a) * qt_Opacity;
          }
          else {
            gl_FragColor = tex * qt_Opacity;
          }
      }"
}
