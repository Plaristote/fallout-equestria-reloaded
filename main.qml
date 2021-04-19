import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings 1.1

Window {
  id: application
  width: 1024
  height: 800
  visible: true
  color: "black"
  title: i18n.t("Fallout Equestria")

  property bool isFullScreen: false
  property bool hasSavedGame: false // TODO
  property string gameLoading
  property int movementMode: 0

  Settings {
    property alias x: application.x
    property alias y: application.y
    property alias width: application.width
    property alias height: application.height
    property alias fullScreen: application.isFullScreen
    property alias movementMode: application.movementMode
  }

  onMovementModeChanged: {
    if (gameManager.movementModeOption !== application.movementMode)
      gameManager.movementModeOption = application.movementMode;
  }
  onIsFullScreenChanged: updateDisplay()
  Component.onCompleted: updateDisplay()

  function updateDisplay() {
    if (isFullScreen)
      application.showFullScreen();
    else
      application.showNormal();
    gameManager.movementModeOption = application.movementMode;
  }

  function createGame() {
    pushView("Game.qml", { initialState: "new-game" });
  }

  function pushView(path, properties = {}) {
    mainView.push(path, properties);
  }

  function popView() {
    mainView.pop();
  }

  function popAllViews() {
    mainView.pop(null);
  }

  StackView {
    id: mainView
    initialItem: "MainMenu.qml"
    anchors.fill: parent

    onCurrentItemChanged: {
      if (depth === 1)
        musicManager.play("mainmenu");
    }
  }

  Timer {
    interval: 500
    running: gameLoading.length > 0
    onTriggered: {
      pushView("Game.qml", { initialState: "load-game" });
      gameManager.loadGame(gameLoading);
      gameLoading = "";
    }
  }

  property alias consoleFontName: consoleFont.name
  property alias titleFontName: titleFont.name

  FontLoader {
    id: consoleFont
    source: "./assets/fonts/JH_FALLOUT.TTF"
  }

  FontLoader {
    id: titleFont
    source: "./assets/fonts/fallout.ttf"
  }
}
