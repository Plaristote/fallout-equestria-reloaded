import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import Game 1.0

Pane {
  property QtObject gameController
  property QtObject character
  id: characterEditor
  background: UiStyle.TerminalPane {}

  function positionChanged() {
    gameController.level.setObjectPosition(character, parseInt(characterXInput.text), parseInt(characterYInput.text))
  }

  signal openInventoryClicked()
  signal removeClicked()

  Grid {
    columns: 2
    spacing: 5

    Label { text: "Position"; color: "green"; }
    Row {
      TerminalField { id: characterXInput; text: character.position.x; onTextChanged: characterEditor.positionChanged() }
      TerminalField { id: characterYInput; text: character.position.y; onTextChanged: characterEditor.positionChanged() }
    }

    Label { text: "Script"; color: "green"; }
    TerminalComboBox {
      id: characterScriptInput
      model: scriptController.getScripts("pnjs")
      onCurrentTextChanged: {
        selectedCharacter.setScript(currentText);
      }
    }

    Label { text: "Dialog"; color: "green"; }
    TerminalComboBox {
      model: scriptController.getScripts("dialogs")
      enabled: false
      //currentIndex: scriptController.getScript("dialogs").indexOf(character.getDialog())
    }

    Label { text: "Sprite"; color: "green"; }
    TerminalComboBox {
      id: characterSpriteInput
      model: animationLibrary.getGroups();
      currentIndex: model.indexOf(character.getSpriteName())
      onCurrentTextChanged: {
        character.setSpriteName(currentText);
      }
    }

    Label { text: "Animation"; color: "green"; }
    TerminalComboBox {
      id: characterAnimationInput
      model: animationLibrary.getAnimationList(characterSpriteInput.currentText);
      currentIndex: model.indexOf(character.getAnimation())
      onCurrentTextChanged: {
        character.setAnimation(currentText);
      }
    }

    TerminalButton {
      Layout.columnSpan: 2
      Layout.alignment: Qt.AlignCenter
      text: "Inventory"
      onClicked: openInventoryClicked()
      padding: 20
      height: 40
    }

    TerminalButton {
      Layout.columnSpan: 2
      Layout.alignment: Qt.AlignCenter
      text: "Remove"
      onClicked: removeClicked()
      padding: 20
      height: 40
    }
  }
}
