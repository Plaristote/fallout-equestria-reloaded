import QtQuick 2.15
import QtQuick.Controls 2.15
import "./assets/ui" as UiStyle
import "game/"
import "game/slideshows/"

Item {
  id: root
  property string currentLevelName
  property bool hasActiveView: application.currentView !== this

  function openLevelView() {
    application.pushView("game/LevelLoaderView.qml", {
      gameController: gameManager.currentGame,
      levelController: gameManager.currentGame.level
    });
  }

  function openWorldmapView() {
    gameManager.currentGame.worldmap.paused = false;
    application.pushView("game/worldmap/Worldmap.qml", { controller: gameManager.currentGame.worldmap })
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
    id: deferredGameOverScreen
    interval: 1000
    onTriggered: {
      gameOverScreen.visible = true;
      while (root.StackView.status === StackView.Inactive)
        application.popView();
    }
  }

  Connections {
    target: gameManager

    function onNewGameStarted() {
      application.pushView("NewGame.qml", {gameController: gameManager.currentGame})
    }

    function onGameLoaded() {
      if (!gameManager.currentGame.level)
        openWorldmapView();
    }

    function onGameOver() {
      application.popAllViews();
    }
  }

  Connections {
    target: gameManager.currentGame

    function onRequestLoadingScreen() {
      while (root.StackView.status === StackView.Inactive)
        application.popView();
    }

    function onLevelChanged() {
      if (gameManager.currentGame.level)
        openLevelView();
      else
        openWorldmapView();
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
    backgroundName: gameManager.currentGame ? gameManager.currentGame.loadingScreenBackground : "loading-screen";
  }

  GameOverScreen {
    id: gameOverScreen
    onClicked: gameManager.endGame()
  }
}
