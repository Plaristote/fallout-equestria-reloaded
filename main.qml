import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
  id: application
  width: 800
  height: 600
  visible: true
  visibility: Window.Maximized
  title: qsTr("Fallout Equestria")

  property bool hasSavedGame: false // TODO

  function createGame() {
    pushView("NewGame.qml");
  }

  function pushView(path) {
    mainView.push(path);
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
