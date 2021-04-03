import QtQuick 2.15

Loader {
  id: root
  property QtObject levelEditor
  property QtObject gameController: levelEditor.gameController

  signal openInventory(QtObject model)

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
        case "InventoryItem":
          component = itemEditor;
          break ;
        default:
          component = dynamicObjectEditor;
          break ;
        }
      }
      objectEditorComponent.sourceComponent = component;
    }

    function onPickedTile(tileX, tileY) {
      root.item.setTilePosition(tileX, tileY);
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
      onRemoveClicked: gameController.level.deleteObject(model)
    }
  }

  Component {
    id: storageEditor
    StorageObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
      onOpenStorageClicked: root.openInventory(selectedObject)
      onRemoveClicked: gameController.level.deleteObject(model)
    }
  }

  Component {
    id: doorwayEditor
    DoorwayObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
      onRemoveClicked: gameController.level.deleteObject(model)
    }
  }

  Component {
    id: itemEditor
    ItemObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
      onRemoveClicked: gameController.level.deleteObject(model)
    }
  }

  Component {
    id: dynamicObjectEditor
    DynamicObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
      onRemoveClicked: gameController.level.deleteObject(model)
    }
  }
}
