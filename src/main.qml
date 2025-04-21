import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtCore
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

  VirtualPointerDisplay {}

  Settings {
    id: settings
    property alias x: application.x
    property alias y: application.y
    property alias width: application.width
    property alias height: application.height
    property alias fullScreen: application.isFullScreen
    property int   movementMode: gameManager.movementModeOption
    property bool  withMovementCursor: mouseCursor.withMoveCursor
    property bool  withPlayerCropCircle: gameManager.withPlayerCropCircle
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
    return mainView.pop() != null;
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
    source: i18n.consoleFont
    pointSize: i18n.consoleFontMetrics["point"]
    tinySize:  i18n.consoleFontMetrics["tiny"]
    bigSize:   i18n.consoleFontMetrics["big"]
    hugeSize:  bigSize / 3 * 4
  }

  property var titleFont: FontStyle {
    id: titleFont
    source: i18n.titleFont
    pointSize: i18n.titleFontMetrics["point"]
    tinySize:  i18n.titleFontMetrics["tiny"]
    bigSize:   i18n.titleFontMetrics["big"]
  }
}
