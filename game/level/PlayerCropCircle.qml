import QtQuick 2.15

ShaderEffect {
  property variant source
  property QtObject levelController
  property QtObject player:   levelController.player
  property point    lastPlayerPosition
  property point    position: Qt.point(0, 0)
  property real     diameter: 90
  property real     offzetX: parent.x
  property real     offzetY: parent.y
  property real     centerX: 0
  property real     centerY: 0
  property bool     withClipping:     player
  property bool     withAmbientColor: levelController.useAmbientLight
  property color    ambientColor:     levelController.ambientColor

  signal positionRefreshed()

  Timer {
    id: positionUpdateTimer
    interval: 10
    repeat: false
    running: player.spritePosition != lastPlayerPosition
    onTriggered: {
      lastPlayerPosition = player.spritePosition;
      updateCenter();
      positionRefreshed();
    }
  }

  function updateCenter() {
    centerX = player.spritePosition.x - offzetX + 35;
    centerY = player.spritePosition.y - offzetY + 10;
  }

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
          lowp float pixelClipped     = withClipping && tex.a != 0.0 && a + b < pow(radius, 2.0) ? 0.0 : 1.0;
          lowp float withAmbientColor = withAmbientColor && tex.a != 0.0 ? 1.0 : 0.0;

          gl_FragColor = vec4(
            pixelClipped * (tex.r + withAmbientColor * (ambientColor.r + tex.r + tex.r) * 0.3),
            pixelClipped * (tex.g + withAmbientColor * (ambientColor.g + tex.g + tex.g) * 0.3),
            pixelClipped * (tex.b + withAmbientColor * (ambientColor.b + tex.b + tex.b) * 0.3),
            pixelClipped * tex.a
          ) * qt_Opacity;
      }"
}
