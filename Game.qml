import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "game/"

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
    else if (mystate == "level") {
      if (gameManager.currentGame.level)
        gameManager.currentGame.endLevel();
      gameManager.currentGame.goToLevel(currentLevelName);
    }
  }

  // Level control
  Timer {
    id: deferredLevelLoading
    interval: 500
    onTriggered: openLevelView();
  }

  Timer {
    id: deferredCharacterCreate
    interval: 500
    onTriggered: application.pushView("NewGame.qml")
  }

  Timer {
    id: deferredGameEnding
    interval: 500
    onTriggered: application.popView();
  }

  Connections {
    target: gameManager

    function onCurrentGameChanged() {
      console.log("(!) Current game changed");
      root.gameController = gameManager.currentGame;
      if (mystate == "new-game")
        mystate = "create-character";
    }
  }

  Connections {
    target: gameManager.currentGame

    function onLevelChanged() {
      if (gameController.level)
        deferredLevelLoading.running = true
      else
        application.popView();
    }
  }

  // Loading screen
  Image {
    source: "assets/backgrounds/default.jpg"
    anchors.fill: parent
    fillMode: Image.PreserveAspectCrop

    Rectangle {
      anchors.centerIn: parent
      width: parent.width
      height: 100
      color: Qt.rgba(0, 0, 0, 0.5)

      Text {
        id: loadingLabel
        anchors.centerIn: parent
        color: "white"
        text: qsTr("Loading")
      }
      Text {
        y: loadingLabel.y
        anchors.left: loadingLabel.right
        text: "..."
        color: "white"
        Timer {
          running: root.visible
          repeat: true
          interval: 300
          onTriggered: parent.text = parent.text.length >= 3 ? '.' : parent.text + '.'
        }
      }
    }
  }
}
