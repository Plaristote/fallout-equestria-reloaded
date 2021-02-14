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
      }
      else if (selectedObject.getObjectType() === "StorageObject") {
        console.log("selected storage object")
        selectedCharacter = null;
      }
      else
      {
        console.log("selected object type", selectedObject.getObjectType());
        selectedCharacter = null;
      }
    }
    else
      selectedObject = selectedCharacter = null;
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

      GameComponents.LevelCanvas {
        id: canvas
        levelController: gameController.level
        renderRoofs: displayRoofCheckbox.checked
        renderWalls: displayWallsCheckbox.checked
        showHoverCoordinates: true
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
        CheckBox {
          id: displayRoofCheckbox
          Text {
            anchors.left: parent.right; anchors.verticalCenter: parent.verticalCenter
            text: "Display roofs"
            color: "white"
          }
        }

        CheckBox {
          id: displayWallsCheckbox
          checked: true
          Text {
            anchors.left: parent.right; anchors.verticalCenter: parent.verticalCenter
            text: "Display walls"
            color: "white"
          }
        }

        Row {
          ComboBox {
            id: objectSelectBox
            model: objectList
          }
          Button {
            text: "Add object"
            onClicked: dialogAddObject.open()
          }
        }

        CharacterObjectEditor {
          id: characterEditor
          visible: selectedCharacter !== null
          character: selectedCharacter
          gameController: root.gameController
          Layout.fillHeight: true
          Layout.fillWidth: true
          onOpenInventoryClicked: {
            characterInventory.character = root.selectedCharacter;
            characterInventory.open();
          }
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

  Dialog {
    id: dialogAddObject
    title: "New object"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Column {
      Row {
        Text { text: "type" }
        ComboBox { id: objectTypeInput; model: ["character", "storage"] }
      }
      Row {
        Text { text: "name" }
        TextField { id: objectNameInput }
      }
      Row {
        Text { text: "character sheet" }
        ComboBox { id: sheetInput; model: scriptController.getCharacterSheets() }
      }
    }
    onAccepted: {
      if (objectTypeInput.currentText === "character") {
        gameController.level.generateCharacter(objectNameInput.text, sheetInput.currentText.replace(".json", ""))
        objectSelectBox.currentIndex = gameController.level.objects.length - 1
      }
      else
        console.log("unhandled type added");
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
