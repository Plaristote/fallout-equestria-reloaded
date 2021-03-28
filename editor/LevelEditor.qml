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

      LevelEditorUi.EditorCanvas {
        id: canvas
        levelController: gameController.level
        renderRoofs: displayRoofCheckbox.checked
        renderWalls: displayWallsCheckbox.checked
        showHoverCoordinates: true
        editorObject: selectedObject

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

        LevelEditorUi.ObjectEditorLoader {
          id: objectEditorComponent
          Layout.fillWidth: true
          levelEditor: root
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

  LevelEditorUi.CharacterInventoryEditor {
    id: characterInventory
    anchors.fill: parent
    anchors.margins: 50
    character: selectedCharacter
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
    onObjectAdded: {
      console.log("Added object", newObject);
      objectSelectBox.currentIndex = gameController.level.objects.indexOf(newObject);
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
