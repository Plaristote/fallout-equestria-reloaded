import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import "./level" as LevelEditorUi
import Game 1.0

Item {
  id: root
  property var gameController
  property alias currentLevelName: levelSelect.currentName
  property var objectList: []
  property QtObject selectedObject

  signal pickedTile(int tileX, int tileY)

  onCurrentLevelNameChanged: {
    selectedObject = null;
    levelEditorUiLoader.sourceComponent = null;
    gameController.loadLevel(currentLevelName.replace(".json", ""), "");
    if (currentLevelName.length > 0)
      levelEditorUiLoader.sourceComponent = levelEditorUi;
  }

  onSelectedObjectChanged: {
    canvas.editorObject = selectedObject;
    objectListComponent.currentName = selectedObject.objectName;
  }

  function updateObjectList() {
    var array = [];
    for (var i = 0 ; i < gameController.level.dynamicObjects.length ; ++i)
      array.push(gameController.level.dynamicObjects[i].objectName);
    objectList = array;
  }

  Connections {
    target: gameController.level
    function onDynamicObjectsChanged() {
      updateObjectList();
    }
    function onClickedOnObject(value) {
      objectSelectBox.currentIndex = objectList.indexOf(value.objectName);
    }
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectPanel {
      id: levelSelect
      model: scriptController.getLevels()
      readOnly: true
    }

    Loader {
      id: levelEditorUiLoader
      Layout.fillHeight: true
      Layout.fillWidth: true
    }
  }

  Component {
    id: levelEditorUi
    RowLayout {
      Component.onCompleted: {
        updateObjectList();
        canvas.translate(-canvas.origin.x, -canvas.origin.y);
      }

      Pane {
        background: UiStyle.TerminalPane {}
        Layout.fillHeight: true
        Layout.fillWidth: true

        LevelEditorUi.EditorCanvas {
          id: canvas
          levelController: gameController.level
          renderRoofs: displayRoofCheckbox.checked
          renderWalls: displayWallsCheckbox.checked
          showHoverCoordinates: true
          editorObject: selectedObject

          editingZone: controlZoneEditor.editingZone
          onToggleZoneTile: controlZoneEditor.toggleTile(tileX, tileY)
          onPickedTile: root.pickedTile(tileX, tileY)
          onPickedObject: objectListComponent.currentName = dynamicObject.objectName;
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
        Layout.preferredWidth: 400
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

          LevelEditorUi.ObjectSelectPanel {
            id: objectListComponent
            model: gameController.level.dynamicObjects
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: selectedObject == null
            onCurrentNameChanged: selectedObject = gameController.level.getObjectByName(currentName);
            onNewClicked: dialogAddObject.open()
            onNewFromTemplateClicked: addTemplateDialog.open()
            onShowClicked: canvas.moveToObject(object)
          }

          LevelEditorUi.ObjectEditorLoader {
            id: objectEditorComponent
            Layout.fillWidth: true
            Layout.fillHeight: true
            levelEditor: root
            visible: selectedObject != null
            onOpenInventory: {
              if (model.getObjectType() === "Character") {
                characterInventory.character = model;
                characterInventory.open();
              }
              else {
                lootEditor.inventory = model.inventory;
                lootEditor.open();
              }
            }
            onSaveTemplateClicked: saveTemplateDialog.open()
            onPreviousClicked: {
              objectListComponent.currentName = "";
              selectedObject = null;
            }
            onShowClicked: canvas.moveToObject(selectedObject)
          }

          ControlZoneEditor {
            id: controlZoneEditor
            selectedObject: root.selectedObject
            displayRoofs: displayRoofCheckbox
            displayWalls: displayWallsCheckbox
            Layout.fillWidth: true
            visible: objectEditorComponent.visible
          }
        }
      }
    }
  }

  LevelEditorUi.SaveTemplateDialog {
    id: saveTemplateDialog
    selectedObject: root.selectedObject
  }

  LevelEditorUi.AddTemplateDialog {
    id: addTemplateDialog
    gameController: root.gameController
    onObjectAdded: selectedObject = newObject
  }

  LevelEditorUi.CharacterInventoryEditor {
    id: characterInventory
    anchors.fill: parent
    anchors.margins: 50
    visible: false
  }

  LevelEditorUi.LootEditor {
    id: lootEditor
    anchors.fill: parent
    anchors.margins: 50
    visible: false
  }

  LevelEditorUi.AddObjectDialog {
    id: dialogAddObject
    gameController: root.gameController
    onObjectAdded: selectedObject = newObject
  }

  MenuButton {
    id: saveButton
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Save"
    onClicked: {
      gameController.save();
      gameController.getDataEngine().saveToFile("./assets/game.json");
    }
  }
}
