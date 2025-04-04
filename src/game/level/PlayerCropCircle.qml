import QtQuick 2.15

ShaderEffect {
  property variant source
  property variant src: source
  property QtObject player
  property point    lastPlayerPosition
  property point    position: Qt.point(0, 0)
  property real     radius: 90 * 2
  property real     smoothEdge: 10
  property real     offzetX: parent.x
  property real     offzetY: parent.y
  property point    centerPoint: Qt.point(0, 0)
  property bool     withClipping:     player

  signal positionRefreshed()

  Timer {
    id: positionUpdateTimer
    interval: 10 + Math.random() * 50
    repeat: false
    running: gameManager.withPlayerCropCircle && player && player.spritePosition != lastPlayerPosition
    onTriggered: {
      lastPlayerPosition = player.spritePosition;
      updateCenter();
      positionRefreshed();
    }
  }

  function updateCenter() {
    centerPoint = Qt.point(
      player.spritePosition.x - offzetX + 35,
      player.spritePosition.y - offzetY + 10
    );
  }

  anchors.fill: parent
  vertexShader: "qrc:/game/level/PlayerCropCircle.shader.vert.qsb"
  fragmentShader: "qrc:/game/level/PlayerCropCircle.shader.frag.qsb"
}
