import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2 as BiDialog
import "qrc:/assets/ui" as UiStyle
import "../ui"
import Game 1.0

Item {
  id: root
  property var objectNames: itemLibrary.getObjectList()
  property var currentObject: ({})
  property alias currentName: objectSelect.currentName
  property var scriptList

  Component.onCompleted: updateScriptList()

  function updateScriptList() {
    scriptList = scriptController.getScripts("items");
  }

  function save()
  {
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

  onCurrentNameChanged: {
    currentObject = itemLibrary.getObject(currentName);
    iconInput.text = currentObject.icon || "";
  }

  onVisibleChanged: objectNames = itemLibrary.getObjectList()

  TextPromptDialog {
    id: newObjectDialog
    title: "New item"
    anchors.centerIn: parent
    function validate() {
      if (objectNames.indexOf(value) >= 0)
        validationError = value + " already exists.";
      else
        validationError = "";
      return validationError === "";
    }

    onAccepted: {
      objectNames.push(value);
      currentName = value;
      objectNamesChanged();
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

  RowLayout {
    anchors.fill: parent

    // ItemSelect
    EditorSelectPanel {
      id: objectSelect
      model: objectNames
      onNewClicked: newObjectDialog.open()
    }
    // END ItemSelect

    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.bottomMargin: saveButton.height

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
    }
  }

  MenuButton {
    id: saveButton
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Save"
    onClicked: root.save()
  }

  TextPromptDialog {
    id: newScriptDialog
    title: "Adding an item script"
    anchors.centerIn: parent
    onAccepted: {
      scriptList.push(value);
      currentObject.script = value;
      scriptListChanged();
      currentObjectChanged();
    }
  }
}
