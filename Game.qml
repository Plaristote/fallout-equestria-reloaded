import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "game/"
import "game/slideshows/"

Item {
  id: root
  property string initialState;
  property string mystate;
  property var gameController;
  property string currentLevelName;
  anchors.fill: parent

  function openLevelView() {
    application.pushView("game/LevelView.qml", {
      gameController: gameController,
      levelController: gameController.level
    });
  }

  Component.onCompleted: {
    mystate = initialState;
  }

  onGameControllerChanged: {
    if (gameController) {
      if (gameController.level)
        deferredLevelLoading.running = true;
    }
    else
      deferredGameEnding.running = true;
  }

  onMystateChanged: {
    console.log("(!) My state changed", mystate);
    if (mystate == "new-game") {
      gameManager.startNewGame();
    }
    else if (mystate == "create-character") {
      deferredCharacterCreate.running = true;
    }
    else if (mystate == "level" && currentLevelName != "") {
      console.log("Game.kml going to open level", currentLevelName);
      gameManager.currentGame.goToLevel(currentLevelName);
    }
    else if (mystate == "worldmap") {
      console.log("Game.kml going to open worldmap");
      deferredWorldmapDisplay = true;
    }
    else
      console.log("/!\\ Game state changed to", mystate, "and I don't know what to do.");
  }

  // Level control
  Timer {
    id: deferredLevelLoading
    interval: 500
    onTriggered: openLevelView();
  }

  Timer {
    property string video
    property int elapsingTime
    id: deferredTransitionLoading
    interval: 500
    onTriggered: {
      application.pushView("game/slideshows/TransitionScreen.qml", {
        gameController:  gameManager.currentGame,
        levelController: gameManager.currentGame.level,
        source: video,
        elapsingTime: elapsingTime
      });
    }
  }

  Timer {
    id: deferredWorldmapDisplay
    interval: 500
    onTriggered: application.pushView("game/worldmap/Worldmap.qml", { controller: root.gameController.worldmap })
  }

  Timer {
    id: deferredCharacterCreate
    interval: 500
    onTriggered: application.pushView("NewGame.qml", {gameController: root.gameController})
  }

  Timer {
    id: deferredGameEnding
    interval: 500
    onTriggered: application.popView();
  }

  Timer {
    id: deferredGameOverScreen
    interval: 1000
    onTriggered: {
      gameOverScreen.visible = true;
      application.popView();
    }
  }

  Connections {
    target: gameManager

    function onCurrentGameChanged() {
      console.log("(!) Current game changed");
      root.gameController = gameManager.currentGame;
      if (mystate == "new-game")
        mystate = "create-character";
      else if (mystate == "load-game" && root.gameController.level)
        mystate = "level";
      else if (mystate == "load-game")
        mystate = "worldmap";
    }
  }

  Connections {
    target: gameManager.currentGame

    function onLevelChanged() {
      console.log("Game.qml onLevelChanged has been called", gameController.level);
      if (gameController.level)
        deferredLevelLoading.running = true;
      else
      {
        deferredWorldmapDisplay.running = true;
        application.popView();
      }
    }

    function onLevelSwapped() {
      console.log("Game.qml onLevelSwapped has been called");
      deferredLevelLoading.running = true;
      application.popView();
    }

    function onGameOver() {
      console.log("Game.qml onGameOver has been called");
      deferredGameOverScreen.running = true;
    }

    function onTransitionRequired(video, elapsingTime) {
      deferredTransitionLoading.video = video;
      deferredTransitionLoading.elapsingTime = elapsingTime;
      deferredTransitionLoading.running = true;
    }
  }

  LoadingScreen {
    visible: !gameOverScreen.visible
  }

  GameOverScreen {
    id: gameOverScreen
    onClicked: gameManager.endGame()
  }
}
