import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import "../hud" as LevelHud
import Game 1.0 as MyGame

Item {
  property QtObject controller // WorldMap
  property point movementStart: controller ? controller.currentPosition : Qt.point(0,0)
  property bool hasOverlay: inventoryViewContainer.activated || mainMenu.activated
  id: root
  state: "default"
  enabled: !gameManager.currentGame.fastPassTime
  //onHasOverlayChanged: controller.paused = hasOverlay || root.state != "default";

  Component.onCompleted: {
    controller.restart();
    musicManager.play("worldmap");
  }

  Connections {
    target: controller

    function onEncounterTriggered(title) { application.popView(); }
    function onEncounterNotify(name, params) {
      gameManager.currentGame.sounds.play("encounter");
      root.state = "randomEncounter";
      if (params.optional) {
        encounterConfirmDialog.text = i18n.t("messages.encounter-title", params) + ' ' + i18n.t("messages.encounter-prompt");
        encounterConfirmDialog.open();
      }
      root.controller.paused = true;
    }

    function onCityEntered() {
      application.popView();
    }
  }

  ConfirmDialog {
    id: encounterConfirmDialog
    anchors.centerIn: parent
    onAccepted: gameManager.currentGame.randomEncounters.triggerScheduledEncounter()
    onRejected: {
      root.state = "default";
      root.controller.restart();
    }
  }

  LevelHud.Actions {
    id: actions
    enabled: application.currentView === root && root.state == "default"
    onMenuTriggered:           mainMenu.toggle()
    onInventoryTriggered:      inventoryViewContainer.toggle()
    onBackTriggered: {
      if  (mainMenu.activated)
        mainMenu.toggle();
      else if (inventoryViewContainer.activated)
        inventoryViewContainer.visible.toggle();
      else if (splashscreen.selectedCity)
        splashscreen.selectedCity = null;
      else
        mainMenu.toggle()
    }
  }

  function clickedOnPlayer() {
    console.log("Clicked on player at", controller.currentPosition, controller.paused);
    if (controller.paused) { return ; }
    for (var i = 0 ; i < controller.cities.length ; ++i) {
      if (controller.cities[i].isInside(controller.currentPosition)) {
        return controller.getIntoCity(controller.cities[i]);
      }
    }
    controller.getIntoWasteland(controller.currentPosition);
  }

  function clickedOnMap() {
    console.log("Clicked on map", worldmapView.mouseX, worldmapView.mouseY, controller.paused);
    if (controller.paused) { return ; }
    movementStart = controller.currentPosition;
    controller.targetPosition = Qt.point(worldmapView.mouseX, worldmapView.mouseY);
  }

  function clickedOnCity(city) {
    if (controller.paused) { return ; }
    if (controller.getCurrentCity() === city)
      controller.getIntoCity(city);
    else {
      movementStart = controller.currentPosition;
      controller.targetPosition = city.position;
    }
  }

  WorldmapView {
    id: worldmapView
    activated: actions.enabled
    anchors { top: parent.top; left: parent.left; bottom: parent.bottom; right: sidebar.left }
    controller: root.controller
    onMapClicked: clickedOnMap()
    Component.onCompleted: centerOn(controller.currentPosition)

    function centerOn(position) {
      scrollTo(position.x - width / 2, position.y - height / 2);
    }

    content: [
      WorldmapCities {
        model: controller ? controller.cities : []
        discoveredCities: controller ? controller.discoveredCities : []
      }
    ]

    Shape {
      anchors.fill: parent
      ShapePath {
        startX: movementStart.x
        startY: movementStart.y
        strokeWidth: 2
        strokeColor: "red"
        strokeStyle: ShapePath.DashLine
        PathLine {
          x: controller ? controller.currentPosition.x : 0;
          y: controller ? controller.currentPosition.y : 0
        }
      }
    }

    Shape {
      id: positionShape
      x: controller ? controller.currentPosition.x - width / 2 : 0
      y: controller ? controller.currentPosition.y - height / 2 : 0
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

    Rectangle {
      id: encounterShape
      visible: false
      x: controller ? controller.currentPosition.x - width / 2 : 0
      y: controller ? controller.currentPosition.y - height / 2 : 0
      width: 50
      height: 50
      radius: width * 0.5
      color: Qt.rgba(255, 0, 0, 0.5);
      border.color: "red"
      border.width: 3
      RotationAnimation on opacity {
        loops: Animation.Infinite
        from: 1
        to:   0.5
      }
    }
  }

  SplashscreenDisplay {
    id: splashscreen
    controller: root.controller
    anchors.left: worldmapView.left
    anchors.right: worldmapView.right
    height: parent.height
    MouseArea { anchors.fill: parent; enabled: splashscreen.selectedCity != null }
  }

  states: [
    State {
      name: "randomEncounter"
      PropertyChanges { target: encounterShape; visible: true }
      PropertyChanges { target: positionShape;  visible: false }
    },
    State {
      name: "default"
      PropertyChanges { target: encounterShape; visible: false }
      PropertyChanges { target: positionShape;  visible: true }
    }
  ]

  WorldmapSidebar {
    id: sidebar
    anchors { right: parent.right; top: parent.top; bottom: parent.bottom }
    controller: root.controller
  }

  LevelHud.PlayerInventory {
    id: inventoryViewContainer
    state: "hidden"
    anchors.fill: parent
  }

  FastTimePassView {}

  LevelHud.Menu {
    id: mainMenu
    anchors.fill: parent
    state: "hidden"
    onActivatedChanged: controller.paused = activated
  }
}
