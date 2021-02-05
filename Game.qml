import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "game/"

Item {
  property QtObject gameController;
  anchors.fill: parent

  function openLevelView() {
    application.pushView("game/LevelView.qml", {
      gameController: gameController,
      levelController: gameController.level
    });
  }

  Component.onCompleted: {
    if (gameController.level)
      deferredLevelLoading.running = true;
  }

  Timer {
    id: deferredLevelLoading
    interval: 500
    onTriggered: openLevelView();
  }

  Connections {
    target: gameController

    function onLevelChanged() {
      if (gameController.level)
        openLevelView();
      else
        application.popView();
    }
  }
}
