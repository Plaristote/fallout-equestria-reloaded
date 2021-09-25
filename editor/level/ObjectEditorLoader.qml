import QtQuick 2.15

Loader {
  id: root
  property QtObject levelEditor
  property QtObject gameController: levelEditor.gameController

  signal showClicked()
  signal openInventory(QtObject model)
  signal previousClicked()
  signal saveTemplateClicked()

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
      root.sourceComponent = component;
    }

    function onPickedTile(tileX, tileY) {
      if (root.item)
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
    }
  }

  Component {
    id: storageEditor
    StorageObjectEditor {
      width: parent.width
      model: selectedObject
      gameController: root.gameController
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
    }
  }
}
