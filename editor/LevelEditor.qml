import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import Game 1.0

Item {
  id: root
  property var gameController
  property alias currentLevelName: levelSelect.currentName
  property var objectList: []
  property alias selectedObjectName: objectSelectBox.currentText
  property QtObject selectedObject
  property QtObject selectedCharacter
  property QtObject selectedDoorway

  onCurrentLevelNameChanged: {
    selectedObject = selectedCharacter = null;
    gameController.goToLevel(currentLevelName.replace(".json", ""));
    updateObjectList();
    canvas.translate(-canvas.origin.x, -canvas.origin.y);
    //gameController.level.paused = true;
  }

  onSelectedObjectNameChanged: {
    if (selectedObjectName !== "") {
      selectedObject = gameController.level.getObjectByName(selectedObjectName);
      if (selectedObject.getObjectType() === "Character") {
        console.log("selected character");
        selectedCharacter = selectedObject
        selectedDoorway = null;
      }
      else if (selectedObject.getObjectType() === "Doorway") {
        console.log("doorway object selected");
        selectedDoorway = selectedObject;
        selectedCharacter = null;
      }
      else
      {
        console.log("selected object type", selectedObject.getObjectType());
        selectedCharacter = selectedDoorway = null;
      }
    }
    else
      selectedObject = selectedCharacter = null;
  }

  onSelectedObjectChanged: {
    canvas.editorObject = selectedObject;
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

    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillHeight: true
      Layout.fillWidth: true

      LevelEditorCanvas {
        id: canvas
        levelController: gameController.level
        renderRoofs: displayRoofCheckbox.checked
        renderWalls: displayWallsCheckbox.checked
        showHoverCoordinates: true

        // Zone edition
        editingZone: controlZoneEditor.editingZone
        onToggleZoneTile: controlZoneEditor.toggleTile(tileX, tileY)
        onPickedObject: {
          objectSelectBox.currentIndex = objectList.indexOf(dynamicObject.objectName);
        }
        onPickedTile: {
          [characterEditor, storageEditor].forEach(function(dynamicObjectEditor) {
            if (dynamicObjectEditor.visible)
              dynamicObjectEditor.setTilePosition(tileX, tileY);
          });
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
      Layout.preferredWidth: 400
      Layout.fillHeight: true

      ColumnLayout {
        width: parent.width

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

        Row {
          Layout.fillWidth: true
          spacing: 5
          SelectBox {
            id: objectSelectBox
            model: objectList
            height: 40
            width: parent.width - 100
          }
          Button {
            text: "Add object"
            onClicked: dialogAddObject.open()
            contentItem: Text {
              text: parent.text
              color: "white"
              font.family: application.titleFontName
              font.pixelSize: 16
              verticalAlignment: Text.AlignVCenter
              horizontalAlignment: Text.AlignHCenter
            }
            background: UiStyle.Label { style: parent.down ? "dark" : "base" }
            width: 100
            height: 40
          }
        }

        CharacterObjectEditor {
          id: characterEditor
          visible: model !== null
          model: selectedObject && selectedObject.getObjectType() === "Character" ? selectedObject : null
          gameController: root.gameController
          Layout.fillWidth: true
          onOpenInventoryClicked: {
            characterInventory.character = characterEditor.model;
            characterInventory.open();
          }
          onRemoveClicked: gameController.level.deleteObject(model)
        }

        StorageObjectEditor {
          id: storageEditor
          visible: model !== null
          model: selectedObject && selectedObject.getObjectType() === "StorageObject" ? selectedObject : null
          gameController: root.gameController
          Layout.fillWidth: true
          onOpenStorageClicked: {
            lootEditor.inventory = selectedObject.inventory;
            lootEditor.open();
          }
          onRemoveClicked: gameController.level.deleteObject(model)
        }

        DoorwayObjectEditor {
          id: doorwayEditor
          visible: model !== null
          model: selectedObject && selectedObject.getObjectType() === "Doorway" ? selectedObject : null
          gameController: root.gameController
          Layout.fillWidth: true
          onRemoveClicked: gameController.level.deleteObject(model)
        }

        ItemObjectEditor {
          id: itemEditor
          visible: model !== null
          model: selectedObject && selectedObject.getObjectType() === "InventoryItem" ? selectedObject : null
          gameController: root.gameController
          Layout.fillWidth: true
          onRemoveClicked: gameController.level.deleteObject(model)
        }

        ControlZoneEditor {
          id: controlZoneEditor
          selectedObject: root.selectedObject
          displayRoofs: displayRoofCheckbox
          displayWalls: displayWallsCheckbox
          Layout.fillWidth: true
        }
      }
    }
  }

  CharacterInventoryEditor {
    id: characterInventory
    anchors.fill: parent
    anchors.margins: 50
    character: selectedCharacter
    visible: false
  }

  LootEditor {
    id: lootEditor
    anchors.fill: parent
    anchors.margins: 50
    visible: false
  }

  Dialog {
    id: dialogAddObject
    title: "New object"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    GridLayout {
      columns: 2
      Text { text: "type" }
      ComboBox { id: objectTypeInput; model: ["character", "storage", "door", "item", "other"] }
      Text { text: "name" }
      TextField { id: objectNameInput }
      // Character inputs
      Text { text: "character sheet"; visible: objectTypeInput.currentText === "character" }
      ComboBox { id: sheetInput; model: scriptController.getCharacterSheets(); visible: objectTypeInput.currentText === "character" }
      // InventoryItem inputs
      Text { text: "type"; visible: objectTypeInput.currentText === "item" }
      ComboBox { id: itemTypeInput; model: itemLibrary.getObjectList(); visible: objectTypeInput.currentText === "item" }
    }
    onAccepted: {
      if (objectTypeInput.currentText === "character")
        gameController.level.generateCharacter(objectNameInput.text, sheetInput.currentText.replace(".json", ""));
      else if (objectTypeInput.currentText == "storage")
        gameController.level.generateStorageObject(objectNameInput.text);
      else if (objectTypeInput.currentText == "door")
        gameController.level.generateDoorway(objectNameInput.text);
      else if (objectTypeInput.currentText == "item")
        gameController.level.generateInventoryItem(objectNameInput.text, itemTypeInput.currentText);
      else {
        console.log("unhandled type added");
        return ;
      }
      objectSelectBox.currentIndex = gameController.level.objects.length - 1;
    }
  }

  MenuButton {
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Save"
    onClicked: {
      gameController.save();
      gameController.getDataEngine().saveToFile("./assets/game.json");
    }
  }
}
