import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs as BiDialog
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
      type: typeInput.currentValue,
      weight: parseInt(weightInput.text),
      value: parseInt(valueInput.text),
      sprite: spriteInput.currentText,
      icon: iconInput.text,
      scriptName: currentObject.scriptName,
      isGroupable: groupableInput.checked
    };
    itemLibrary.setObject(currentName, currentObject);
    itemLibrary.save();
    //scriptController.setScript("items", currentObject.script, scriptEditor.text);
  }

  GridLayout {
    id: formGrid
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    columns: 2

    TerminalLabel {
      text: i18n.t("game-editor.items.type")
    }

    TerminalComboBox {
      Layout.fillWidth: true
      id: typeInput
      valueRole: "value"
      textRole: "text"
      model: [
        { value: "weapon", text: i18n.t("item-type.weapon") },
        { value: "armor", text: i18n.t("item-type.armor") },
        { value: "ammo", text: i18n.t("item-type.ammo") },
        { value: "consommables", text: i18n.t("item-type.consommables") },
        { value: "keys", text: i18n.t("item-type.keys") },
        { value: "misc", text: i18n.t("item-type.misc") }
      ]
      currentIndex: indexOfValue(currentObject.type)
    }

    TerminalLabel {
      text: i18n.t("game-editor.items.weight")
    }

    TerminalField {
      Layout.fillWidth: true
      id: weightInput
      text: currentObject.weight
    }

    TerminalLabel {
      text: i18n.t("game-editor.items.value")
    }

    TerminalField {
      Layout.fillWidth: true
      id: valueInput
      text: currentObject.value
    }

    TerminalLabel {
      text: i18n.t("game-editor.items.groupable")
    }

    TerminalCheckBox {
      id: groupableInput
      checked: currentObject.isGroupable
    }

    TerminalLabel {
      text: i18n.t("game-editor.sprite")
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
      text: i18n.t("game-editor.items.icon")
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
        text: i18n.t("game-editor.pick")
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

    ScriptInputField {
      Layout.fillWidth: true
      scriptCategory: "items"
      model: currentObject
      onValueChanged: currentObject.scriptName = newValue
    }
  } // END Grid Layout

  BiDialog.FileDialog {
    property var target: iconInput
    id: iconPicker
    title: i18n.t("game-editor.file-picker-title")
    currentFolder: rootPath + "assets/icons"
    nameFilters: ["Image files (*.jpg, *.png, *.webp)"]

    onAccepted: {
      const path = iconPicker.fileUrl.toString().replace(/.*\/assets\/icons\//, "")
      target.text = path;
    }
  }
}
