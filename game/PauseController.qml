import QtQuick 2.12

Item {
  property bool paused: false

  onPausedChanged: {
    if (gameManager.currentGame) {
      if (gameManager.currentGame.level)
        gameManager.currentGame.level.paused = paused;
      else
        gameManager.currentGame.worldmap.paused = paused;
    }
  }
}
