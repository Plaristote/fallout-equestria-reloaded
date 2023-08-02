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

  readonly property int shWithClipping:     withClipping ? 1 : 0
  readonly property int shWithAmbientColor: withAmbientColor ? 1 : 0

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
    centerX = player.spritePosition.x - offzetX + 35;
    centerY = player.spritePosition.y - offzetY + 10;
  }

  anchors.fill: parent
  vertexShader: "qrc:/game/level/PlayerCropCircle.shader.vert.qsb"
  fragmentShader: "qrc:/game/level/PlayerCropCircle.shader.frag.qsb"
}
