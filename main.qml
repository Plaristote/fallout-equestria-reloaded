import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
  id: application
  width: 1024
  height: 800
  visible: true
  //visibility: Window.Maximized
  color: "black"
  title: qsTr("Fallout Equestria")

  property bool isMaximizeed: visibility === Window.Maximized
  property bool hasSavedGame: false // TODO
  property string gameLoading

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
