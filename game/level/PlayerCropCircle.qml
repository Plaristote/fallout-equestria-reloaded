import QtQuick 2.15

ShaderEffect {
  property variant source
  property QtObject levelController
  property QtObject player:   levelController.player
  property point    position: player ? player.spritePosition : Qt.point(0, 0)
  property real     diameter: 90
  property real     offzetX: parent.x
  property real     offzetY: parent.y
  property real     centerX: position.x - offzetX + 35
  property real     centerY: position.y - offzetY + 10
  property bool     withClipping:     player
  property bool     withAmbientColor: levelController.useAmbientLight
  property color    ambientColor:     levelController.ambientColor

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
      uniform lowp float qt_Opacity;
      uniform lowp float centerX;
      uniform lowp float centerY;
      uniform lowp float diameter;
      uniform lowp float width;
      uniform lowp float height;
      uniform bool withClipping;
      uniform bool withAmbientColor;
      uniform vec4 ambientColor;
      void main() {
          lowp vec4 tex = texture2D(source, coord);
          lowp float x = coord.x * width;
          lowp float y = coord.y * height;
          lowp float a = pow(x - centerX, 2.0);
          lowp float b = pow(y - centerY, 2.0);
          lowp float radius = diameter / 2.0;

          if (withClipping && tex.a != 0.0 && a + b < pow(radius, 2.0)) {
            gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
          }
          else if (withAmbientColor && tex.a != 0.0) {
            lowp float r = ambientColor.r + tex.r + tex.r;
            lowp float g = ambientColor.g + tex.g + tex.g;
            lowp float b = ambientColor.b + tex.b + tex.b;
            gl_FragColor = vec4(r * 0.3, g * 0.3, b * 0.3, tex.a) * qt_Opacity;
          }
          else {
            gl_FragColor = tex * qt_Opacity;
          }
      }"
}
