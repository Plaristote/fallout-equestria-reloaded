import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "game/"

Item {
  id: root
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

  // Level control
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
