import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
  id: application
  width: 1024
  height: 800
  visible: true
  //visibility: Window.Maximized
  title: qsTr("Fallout Equestria")

  property bool isMaximizeed: visibility === Window.Maximized
  property bool hasSavedGame: false // TODO

  function createGame() {
    //pushView("NewGame.qml");
    gameController.goToLevel("eltest5");
    pushView("Game.qml", { gameController: gameController });
  }

  function pushView(path, properties = {}) {
    mainView.push(path, properties);
  }

  function popView() {
    mainView.pop();
  }

  StackView {
    id: mainView
    initialItem: "MainMenu.qml"
    anchors.fill: parent
  }
}
