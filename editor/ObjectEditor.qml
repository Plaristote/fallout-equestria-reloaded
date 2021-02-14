import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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
      script: scriptInput.currentText,
      isGroupable: groupableInput.checked
    };
    itemLibrary.setObject(currentName, currentObject);
    itemLibrary.save();
    scriptController.setScript("items", currentObject.script, scriptEditor.text);
  }

  onCurrentNameChanged: {
    currentObject = itemLibrary.getObject(currentName);
  }

  onVisibleChanged: objectNames = itemLibrary.getObjectList()

  Dialog {
    id: newObjectDialog
    title: "New item"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "name" }
      TextField { id: newObjectName }
    }
    onAccepted: {
      objectNames.push(newObjectName.text);
      currentName = newObjectName.text;
      objectNamesChanged();
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

      Flickable {
        clip: true
        anchors.fill: parent
        contentHeight: formGrid.height

        GridLayout {
          id: formGrid
          width: parent.width
          columns: 2

          Label {
            text: "type"
            color: "green"
          }

          TerminalComboBox {
            id: typeInput
            model: ["weapon","armor","ammo","consommables","misc"]
            currentIndex: model.indexOf(currentObject.type)
          }

          Label {
            text: "weight"
            color: "green"
          }

          TerminalField {
            id: weightInput
            text: currentObject.weight
          }

          Label {
            text: "value"
            color: "green"
          }

          TerminalField {
            id: valueInput
            text: currentObject.value
          }

          Label {
            text: "groupable"
            color: "green"
          }

          CheckBox {
            id: groupableInput
            checked: currentObject.isGroupable
          }

          Label {
            text: "sprite"
            color: "green"
          }

          Row {
            TerminalComboBox {
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

          Label {
            id: labelScript
            text: "script"
            color: "green"
          }

          Item {
            Layout.preferredWidth: parent.width - labelScript.width - 100
            Layout.preferredHeight: 700

            Row {
              id: scriptHeader
              anchors.top: parent.top
              anchors.left: parent.left

              TerminalComboBox {
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

            Rectangle {
              anchors.top: scriptHeader.bottom
              anchors { bottom: parent.bottom; right: parent.right; left: parent.left }
              border.width: 1
              border.color: "green"
              color: "transparent"
              TextEdit {
                id: scriptEditor
                anchors.fill: parent
                color: "white"
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
    }
  }

  Dialog {
    id: newScriptDialog
    standardButtons: Dialog.Ok | Dialog.Cancel
    modal: true
    anchors.centerIn: parent
    Row {
      Label { text: "filename" }
      TextField { id: filenameInput }
    }
    onAccepted: {
      scriptList.push(filenameInput.text);
      currentObject.script = filenameInput.text;
      scriptListChanged();
      currentObjectChanged();
    }
  }
}
