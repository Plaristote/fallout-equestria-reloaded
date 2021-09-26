import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2 as BiDialog
import "qrc:/assets/ui" as UiStyle
import "../ui"
import Game 1.0

Pane {
  property var scriptList: []
  property var currentObject: ({})
  background: UiStyle.TerminalPane {}

  Component.onCompleted: {
    updateScriptList();
    iconInput.text = currentObject.icon || "";
  }

  function updateScriptList() {
    scriptList = scriptController.getScripts("items");
  }

  function save() {
    currentObject = {
      type: typeInput.currentText,
      weight: parseInt(weightInput.text),
      value: parseInt(valueInput.text),
      sprite: spriteInput.currentText,
      icon: iconInput.text,
      script: scriptInput.currentText,
      isGroupable: groupableInput.checked
    };
    itemLibrary.setObject(currentName, currentObject);
    itemLibrary.save();
    scriptController.setScript("items", currentObject.script, scriptEditor.text);
  }

  GridLayout {
    id: formGrid
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    columns: 2

    TerminalLabel {
      text: "type"
    }

    TerminalComboBox {
      Layout.fillWidth: true
      id: typeInput
      model: ["weapon","armor","ammo","consommables","misc"]
      currentIndex: model.indexOf(currentObject.type)
    }

    TerminalLabel {
      text: "weight"
    }

    TerminalField {
      Layout.fillWidth: true
      id: weightInput
      text: currentObject.weight
    }

    TerminalLabel {
      text: "value"
    }

    TerminalField {
      Layout.fillWidth: true
      id: valueInput
      text: currentObject.value
    }

    TerminalLabel {
      text: "groupable"
    }

    TerminalCheckBox {
      id: groupableInput
      checked: currentObject.isGroupable
    }

    TerminalLabel {
      text: "sprite"
    }

    RowLayout {
      Layout.fillWidth: true
      TerminalComboBox {
        Layout.fillWidth: true
        id: spriteInput
        model: animationLibrary.getAnimationList("items")
        currentIndex: animationLibrary.getAnimationList("items").indexOf(currentObject.sprite)
        onCurrentTextChanged: {
          itemSprite.initialize("items", currentText);
          console.log("source", itemSprite.source);
        }
      }

      Image {
        SpriteAnimation {
          id: itemSprite
        }
        source: fileProtocol + itemSprite.source
        width: itemSprite.clippedRect.width
        height: itemSprite.clippedRect.height
        sourceClipRect: itemSprite.clippedRect
      }
    }

    TerminalLabel {
      text: "icon"
    }

    RowLayout {
      Layout.fillWidth: true
      TerminalField {
        id: iconInput
        Layout.fillWidth: true
        text: currentObject.icon
        readOnly: true
      }
      TerminalButton {
        text: "Pick"
        onClicked: iconPicker.open()
      }
      Image {
        source: assetPath + "/icons/" + iconInput.text
        height: 40
        width: 40
      }
    }

    TerminalLabel {
      id: labelScript
      text: "script"
    }

    RowLayout {
      Layout.fillWidth: true
      id: scriptHeader
      spacing: 5

      TerminalComboBox {
        Layout.fillWidth: true
        id: scriptInput
        model: scriptList
        currentIndex: scriptList.indexOf(currentObject.script)
        onCurrentTextChanged: {
          scriptEditor.text = scriptController.getScript("items", scriptInput.currentText)
        }
      }

      TerminalButton {
        text: "Add script"
        height: scriptInput.height
        onClicked: {
          newScriptDialog.open();
        }
      }
    }
  } // END Grid Layout

  Rectangle {
    anchors.top: formGrid.bottom
    anchors { left: parent.left; bottom: parent.bottom; right: parent.right }
    border.width: 1
    border.color: "green"
    color: "transparent"
    ScrollView {
      anchors.fill: parent;
      clip: true
      contentHeight: scriptEditor.contentHeight
      contentWidth: scriptEditor.contentWidth
      TextEdit {
        id: scriptEditor
        padding: 5
        color: "white"
        selectByMouse: true
      }
    }
  }

  TextPromptDialog {
    id: newScriptDialog
    title: "Adding an item script"
    anchors.centerIn: parent
    onAccepted: {
      if (scriptList.indexOf(newScriptDialog.value) < 0) {
        const newName = scriptController.setScript("items", newScriptDialog.value, "");
        updateScriptList();
        scriptInput.currentIndex = scriptList.indexOf(newName);
      }
      else
        scriptInput.currentIndex = scriptList.indexOf(newScriptDialog.value);
    }
  }

  BiDialog.FileDialog {
    property var target: iconInput
    id: iconPicker
    title: "Please chose a file"
    folder: rootPath + "assets/icons"
    nameFilters: ["Image files (*.jpg, *.png, *.webp)"]

    onAccepted: {
      const path = iconPicker.fileUrl.toString().replace(/.*\/assets\/icons\//, "")
      console.log("File selected", path);
      target.text = path;
    }
  }
}
