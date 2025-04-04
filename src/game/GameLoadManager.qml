import QtQuick 2.15

Item {
  property string gameLoading

  onGameLoadingChanged: {
    if (gameLoading.length > 0) {
      if (gameManager.currentGame)
        gameManager.endGame();
      else
        gameLoaderTimer.running = true;
    }
  }

  Connections {
    target: gameManager
    function onGameOver() {
      if (gameLoading.length > 0)
        gameLoaderTimer.running = true;
    }
  }

  Timer {
    id: gameLoaderTimer
    interval: 150
    onTriggered: {
      application.pushView("Game.qml");
      gameManager.loadGame(gameLoading);
      gameLoading = "";
    }
  }
}
