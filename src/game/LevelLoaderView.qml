import QtQuick 2.15

Loader {
  id: root
  property QtObject gameController
  property QtObject levelController

  sourceComponent: levelController ? levelComponent : null

  Connections {
    target: gameController
    function onLevelDestroy() { root.levelController = null }
  }

  Component {
    id: levelComponent
    LevelView {
      gameController: root.gameController
      levelController: root.levelController
    }
  }
}
