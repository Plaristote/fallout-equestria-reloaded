import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15
import "../assets/ui" as UiStyle
import Game 1.0 as MyGame

Item {
  property QtObject controller // WorldMap
  property point movementStart: controller.currentPosition
  id: root

  function clickedOnPlayer() {
    console.log("Clicked on player at", controller.currentPosition);
    //application.popView();
    for (var i = 0 ; i < controller.cities.length ; ++i) {
      if (controller.cities[i].isInside(controller.currentPosition)) {
        application.popView();
        return controller.getIntoCity(controller.cities[i]);
      }
    }
    controller.getIntoWasteland(controller.currentPosition);
  }

  function clickedOnMap() {
    console.log("Clicked on map", worldmapView.mouseX, worldmapView.mouseY);
    movementStart = controller.currentPosition;
    controller.targetPosition = Qt.point(worldmapView.mouseX, worldmapView.mouseY);
  }

  function clickedOnCity(city) {
    movementStart = controller.currentPosition;
    controller.targetPosition = city.position;
  }

  WorldmapView {
    id: worldmapView
    anchors { top: parent.top; left: parent.left; bottom: parent.bottom; right: sidebar.left }
    controller: root.controller
    onMapClicked: clickedOnMap()

    Shape {
      anchors.fill: parent
      ShapePath {
        startX: movementStart.x
        startY: movementStart.y
        strokeWidth: 2
        strokeColor: "red"
        strokeStyle: ShapePath.DashLine
        PathLine { x: controller.currentPosition.x; y: controller.currentPosition.y }
      }
    }

    Shape {
      id: positionShape
      x: controller.currentPosition.x - width / 2; y: controller.currentPosition.y - height / 2
      width: 24
      height: 12
      ShapePath {
        strokeWidth: positionMouseArea.containsMouse ? 6 : 4
        strokeColor: "yellow"
        fillColor: Qt.rgba(255, 255, 0, positionMouseArea.containsMouse ? 1 : 0.5)
        startX: 0
        startY: 0
        PathLine { x: positionShape.width; y: 0 }
        PathLine { x: positionShape.width / 2; y: positionShape.height }
        PathLine { x: 0; y: 0}
      }
      MouseArea {
        id: positionMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: clickedOnPlayer()
      }
    }

    WorldmapCities {
      model: controller.cities
    }
  }

  Pane {
    id: sidebar
    background: UiStyle.Pane {}
    anchors { right: parent.right; top: parent.top; bottom: parent.bottom }

    Connections {
      target: controller.timeManager
      function onDateChanged() {
        dateText.text = dateText.renderDateText();
      }
    }

    Image {
      id: clockView
      source: "qrc:/assets/ui/clock.png"
      anchors.top: parent.top
      anchors.horizontalCenter: parent.horizontalCenter
      width: 150
      height: 150
      rotation: (controller.timeManager.hour + (controller.timeManager.minute / 60)) * -15 -220
    }

    Image {
      source: "qrc:/assets/ui/clock-overlay.png"
      anchors.fill: clockView
    }

    Pane {
      background: UiStyle.TerminalPane {}
      height: clockView.height / 2 + 10
      anchors.left: clockView.left
      anchors.right: clockView.right
      anchors.bottom: clockView.bottom

      Text {
        id: dateText
        text: renderDateText()
        color: "white"
        anchors.centerIn: parent

        function renderDateText() {
          return controller.timeManager.year + "/"
              + ('0' + controller.timeManager.month).slice(-2) + "/"
              + ('0' + controller.timeManager.day).slice(-2) + " "
              + ('0' + controller.timeManager.hour).slice(-2) + ":"
              + ('0' + controller.timeManager.minute).slice(-2);
        }
      }
    }

    Pane {
      background: UiStyle.Pane {}
      anchors.top: clockView.bottom
      anchors.topMargin: 20
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom

      Column {
        Repeater {
          model: controller.cities
          delegate: Row {
            UiStyle.TinyButton {
              id: cityButton
              onClicked: clickedOnCity(controller.cities[index])
            }
            Label {
              height: cityButton.height
              text: controller.cities[index].name
              color: "white"
              padding: 5
              background: UiStyle.Label {}
            }
          }
        }
      }
    }
  }
}
