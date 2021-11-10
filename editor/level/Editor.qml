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

  RowLayout {
    anchors.fill: parent

    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillHeight: true
      Layout.fillWidth: true

      EditorCanvas {
        id: canvas
        anchors.fill: parent
        levelController: root.gameController.level
        showHoverCoordinates: true
        editorObject: selectedObject || selectedGroup
        editingZone:  childrenView.controlZone.editingZone
        onPickedTile: {
          if (editingZone)
            childrenView.controlZone.toggleTile(tileX, tileY)
          else
            root.pickedTile(tileX, tileY)
        }
        onPickedObject: {
          root.selectedGroup  = dynamicObject.parent;
          root.selectedObject = dynamicObject;
        }
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

        RowLayout {
          CheckBox {
            id: displayRoofCheckbox
            text: "Display roofs"
            checked: true
            onCheckedChanged: canvas.renderTarget.renderRoofs = checked
            contentItem: Text {
              leftPadding: 45
              text: parent.text
              color: "white"
            }
          }

          CheckBox {
            id: displayWallsCheckbox
            text: "Display walls"
            checked: true
            onCheckStateChanged: canvas.renderTarget.renderWalls = checked
            contentItem: Text {
              leftPadding: 45
              text: parent.text
              color: "white"
            }
          }

          Label {
            text: "Floor"
            color: "white"
          }

          TextField {
            id: currentFloorInput
            background: UiStyle.Label {}
            onTextChanged: gameController.level.currentFloor = parseInt(text)
            color: "white"
            Connections {
              target: gameController.level
              function onFloorChanged() { currentFloorInput.text = gameController.level.currentFloor.toString(); }
            }
          }
        }

        TreeEditorPanel {
          id: childrenView
          levelEditor: root
          currentGroup: root.gameController.level
          Layout.fillWidth: true
          Layout.fillHeight: true
          onNewObjectClicked: dialogAddObject.openWithGroup(currentGroup)
          onNewGroupClicked:  dialogAddGroup.openWithGroup(currentGroup)
          onShowClicked:      canvas.camera.moveToObject(object)
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
