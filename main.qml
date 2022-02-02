import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings 1.1
import "ui"
import "game"

Window {
  id: application
  width: 1024
  height: 800
  visible: true
  color: "black"
  title: i18n.t("Fallout Equestria")

  property bool isFullScreen: true
  property bool hasSavedGame: false // TODO
  property alias gameLoading: gameLoadManager.gameLoading
  property alias currentView: mainView.currentItem

  Settings {
    id: settings
    property alias x: application.x
    property alias y: application.y
    property alias width: application.width
    property alias height: application.height
    property alias fullScreen: application.isFullScreen
    property int   movementMode: gameManager.movementModeOption
    property bool  withMovementCursor: mouseCursor.withMoveCursor
    property real  combatSpeed:  gameManager.combatSpeedOption
  }

  onIsFullScreenChanged: updateDisplay()
  Component.onCompleted: updateDisplay()

  function updateDisplay() {
    if (isFullScreen)
      application.showFullScreen();
    else
      application.showNormal();
    gameManager.movementModeOption = settings.movementMode;
    gameManager.combatSpeedOption = settings.combatSpeed;
    mouseCursor.withMoveCursor = settings.withMovementCursor;
  }

  function createGame() {
    pushView("Game.qml");
    gameManager.startNewGame();
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

  GameLoadManager { id: gameLoadManager }

  Shortcut {
    sequence: "F11"
    onActivated: application.isFullScreen = !application.isFullScreen
  }

  property alias consoleFontName: consoleFont.name
  property alias titleFontName: titleFont.name

  property var consoleFont: FontStyle {
    id: consoleFont
    source: "./assets/fonts/JH_FALLOUT.TTF"
    pointSize: 9
    tinySize:  8
    bigSize:   12
  }

  property var titleFont: FontStyle {
    id: titleFont
    source: "./assets/fonts/fallout.ttf"
    pointSize: 18
    tinySize:  24
    bigSize:   32
  }
}
