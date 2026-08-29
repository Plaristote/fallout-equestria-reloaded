import QtQuick 2.15

Loader {
  id: root
  property QtObject levelEditor
  property QtObject gameController: levelEditor.gameController
  property var nextComponent: null

  signal showClicked()
  signal openInventory(QtObject model)
  signal requestSpriteView(string group)
  signal requestCharacterView(string characterSheet)
  signal requestDialogView(string dialogName)
  signal previousClicked()
  signal saveTemplateClicked()

  Timer {
    interval: 50
    running: nextComponent
    onTriggered: {
      root.sourceComponent = nextComponent;
      root.nextComponent = null;
    }
  }

  Connections {
    target: levelEditor

    function onSelectedObjectChanged() {
      var component = null;

      if (target.selectedObject) {
        switch (target.selectedObject.getObjectType()) {
        case "Character":
          component = characterEditor;
          break ;
        case "StorageObject":
          component = storageEditor;
          break ;
        case "Doorway":
          component = doorwayEditor;
          break ;
        case "Elevator":
          component = elevatorEditor;
          break ;
        case "InventoryItem":
          component = itemEditor;
          break ;
        default:
          component = dynamicObjectEditor;
          break ;
        }
      }
      root.sourceComponent = null;
      root.nextComponent = component;
    }

    function onPickedTile(tileX, tileY) {
      if (root.item)
        root.item.setTilePosition(tileX, tileY, gameController.level.currentFloor);
    }
  }

  Component {
    id: characterEditor
    CharacterObjectEditor {
      visible: model !== null
      width: parent.width
      model: selectedObject && selectedObject.getObjectType() === "Character" ? selectedObject : null
      gameController: root.gameController
      onOpenInventoryClicked: root.openInventory(selectedObject)
      onRequestCharacterView: root.requestCharacterView(characterSheet)
      onRequestDialogView:    root.requestDialogView(dialogName)
    }
  }

  Component {
    id: storageEditor
    StorageObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
      withCover: true
      onOpenStorageClicked: root.openInventory(selectedObject)
    }
  }

  Component {
    id: doorwayEditor
    DoorwayObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
    }
  }

  Component {
    id: elevatorEditor
    ElevatorObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
    }
  }

  Component {
    id: itemEditor
    ItemObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
    }
  }

  Component {
    id: dynamicObjectEditor
    DynamicObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
      withPathBlocking: true
      withCover: true
      withInteractive: true
    }
  }
}
