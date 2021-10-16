import QtQuick 2.15
import QtQuick.Controls 2.15
import "./assets/ui" as UiStyle
import "game/"
import "game/slideshows/"

Item {
  id: root
  property var gameController
  property string currentLevelName
  property bool hasActiveView: application.currentView !== this

  function openLevelView() {
    application.pushView("game/LevelView.qml", {
      gameController: gameController,
      levelController: gameController.level
    });
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
    onTriggered: {
      gameManager.currentGame.worldmap.paused = false;
      application.pushView("game/worldmap/Worldmap.qml", { controller: root.gameController.worldmap })
    }
  }

  Timer {
    id: deferredCharacterCreate
    interval: 500
    onTriggered: application.pushView("NewGame.qml", {gameController: root.gameController})
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
    }

    function onNewGameStarted() {
      deferredCharacterCreate.running = true;
    }

    function onGameLoaded() {
      if (gameManager.currentGame.level)
        deferredLevelLoading.running = true;
      else
        deferredWorldmapDisplay.running = true;
    }

    function onGameOver() {
      application.popView();
    }
  }

  Connections {
    target: gameManager.currentGame

    function onLevelChanged() {
      console.log("Game.qml onLevelChanged has been called", gameController.level);
      if (gameController.level)
        deferredLevelLoading.running = true;
      else
        deferredWorldmapDisplay.running = true;
      if (root.hasActiveView)
        application.popView();
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

    function onLoadError(message) {
      errorDialog.text = message;
      errorDialog.open();
    }
  }

  UiStyle.CustomDialog {
    property alias text: errorMessage.text
    id: errorDialog
    modal: true
    parent: Overlay.overlay
    standardButtons: Dialog.Ok
    anchors.centerIn: parent

    Label {
      id: errorMessage
      color: "white"
      font.family: application.titleFontName
      font.pixelSize: 16
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
