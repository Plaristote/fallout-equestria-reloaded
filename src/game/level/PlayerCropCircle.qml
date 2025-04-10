import QtQuick 2.15

ShaderEffect {
  property variant source
  property variant src: source
  property QtObject player
  property point    lastPlayerPosition
  property point    position:     Qt.point(0, 0)
  property size     texSize:      Qt.size(width, height)
  property real     radius:       47 * 1.5
  property real     smoothEdge:   radius * 0.4
  property real     offzetX:      parent.x
  property real     offzetY:      parent.y
  property point    offset:       Qt.point(offzetX, offzetY)
  property point    centerPoint:  Qt.point(0, 0)
  property bool     withClipping: player
  property bool     autoRefresh:  true

  signal positionRefreshed()

  Loader {
    sourceComponent: autoRefresh ? positionUpdateTimer : null
  }

  Component {
    id: positionUpdateTimer
    Timer {
      interval: 10 + Math.random() * 50
      repeat: false
      running: gameManager.withPlayerCropCircle && player && player.spritePosition != lastPlayerPosition
      onTriggered: {
        lastPlayerPosition = player.spritePosition;
        updateCenter();
        positionRefreshed();
      }
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
