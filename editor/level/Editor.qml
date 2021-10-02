import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents

Item {
  id: root
  property QtObject gameController
  property alias selectedObject: childrenView.currentObject
  property alias selectedGroup: childrenView.currentGroup

  signal pickedTile(int tileX, int tileY)
  signal requestCharacterView(string characterSheet)
  signal requestSpriteView(string group)
  signal requestDialogView(string dialogName)

  Connections {
    target: gameController.level
    function onClickedOnObject(value) {
      selectedObject = value;
    }
  }

  Component.onCompleted: canvas.translate(-canvas.origin.x, -canvas.origin.y);

  RowLayout {
    anchors.fill: parent

    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillHeight: true
      Layout.fillWidth: true

      EditorCanvas {
        id: canvas
        levelController: gameController.level
        renderRoofs: displayRoofCheckbox.checked
        renderWalls: displayWallsCheckbox.checked
        showHoverCoordinates: true
        editorObject:     selectedObject || selectedGroup
        editingZone:      childrenView.controlZone.editingZone
        onToggleZoneTile: childrenView.controlZone.toggleTile(tileX, tileY)
        onPickedTile: root.pickedTile(tileX, tileY)
        onPickedObject: {
          root.selectedGroup  = dynamicObject.parent;
          root.selectedObject = dynamicObject;
        }
      }

      GameComponents.ScreenEdges {
        enabled: characterInventory.visible == false
        onMoveTop:    { canvas.translate(0, scrollSpeed); }
        onMoveLeft:   { canvas.translate(scrollSpeed, 0); }
        onMoveRight:  { canvas.translate(-scrollSpeed, 0); }
        onMoveBottom: { canvas.translate(0, -scrollSpeed); }
      }
    }

    Pane {
      background: UiStyle.Pane {}
      Layout.preferredWidth: 500
      Layout.fillHeight: true
      Layout.bottomMargin: saveButton.height

      ColumnLayout {
        width: parent.width
        height: parent.height

        Row {
          CheckBox {
            id: displayRoofCheckbox
            text: "Display roofs"
            contentItem: Text {
              leftPadding: 45
              text: parent.text
              color: "white"
            }
          }

          CheckBox {
            id: displayWallsCheckbox
            checked: true
            text: "Display walls"
            contentItem: Text {
              leftPadding: 45
              text: parent.text
              color: "white"
            }
          }
        }

        TreeEditorPanel {
          id: childrenView
          levelEditor: root
          currentGroup: root.gameController.level
          Layout.fillWidth: true
          Layout.fillHeight: true
          //visible: currentObject == null
          onNewObjectClicked: dialogAddObject.openWithGroup(currentGroup)
          onNewGroupClicked:  dialogAddGroup.openWithGroup(currentGroup)
          onShowClicked:      canvas.moveToObject(object)
          onRemoveObjectClicked: {
            currentGroup = object.parent;
            gameController.level.deleteObject(object);
          }
          onRemoveGroupClicked:  {
            currentGroup = object.parent;
            gameController.level.deleteGroup(object);
          }
          onRequestCharacterView: root.requestCharacterView(characterSheet)
          onRequestSpriteView: root.requestSpriteView(group)
          onRequestDialogView: root.requestDialogView(dialogName)
          onOpenInventory: {
            if (object.getObjectType() === "Character") {
              characterInventory.character = object;
              characterInventory.open();
            }
            else {
              lootEditor.inventory = object.inventory;
              lootEditor.open();
            }
          }
        }
      }
    }
  }

  SaveTemplateDialog {
    id: saveTemplateDialog
    selectedObject: root.selectedObject
  }

  AddTemplateDialog {
    id: addTemplateDialog
    gameController: root.gameController
    onObjectAdded: selectedObject = newObject
  }

  CharacterInventoryEditor {
    id: characterInventory
    anchors.fill: parent
    anchors.margins: 50
    visible: false
  }

  LootEditor {
    id: lootEditor
    anchors.fill: parent
    anchors.margins: 50
    visible: false
  }

  AddObjectDialog {
    id: dialogAddObject
    onObjectAdded: selectedObject = newObject
  }

  AddGroupDialog {
    id: dialogAddGroup
  }
}
