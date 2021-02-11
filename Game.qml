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
    id: deferredWorldmapDisplay
    interval: 500
    onTriggered: application.pushView("game/Worldmap.qml", { controller: root.gameController.worldmap })
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
      console.log("Game.kml onLevelChanged haz been called");
      if (gameController.level)
        deferredLevelLoading.running = true;
      else
      {
        console.log("Zuce mon zboub");
        deferredWorldmapDisplay.running = true;
        application.popView();
      }
    }

    function onLevelSwapped() {
      deferredLevelLoading.running = true;
      application.popView();
    }
  }

  // Loading screen
  Image {
    source: "assets/backgrounds/ministry-of-peace.jpg"
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
