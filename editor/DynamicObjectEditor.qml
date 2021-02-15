import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import Game 1.0

Pane {
  property QtObject gameController
  property QtObject model
  property string scriptCategory: "behaviours"
  property alias fields: additionalFields.children
  id: objectEditor
  background: UiStyle.TerminalPane {}

  function positionChanged() {
    const gridX = parseInt(gridXInput.text);
    const gridY = parseInt(gridYInput.text);
    gameController.level.setObjectPosition(model, gridX, gridY);
    if (model.floating)
      renderPositionChanged();
  }

  function renderPositionChanged() {
    const renderX = parseInt(renderXInput.text);
    const renderY = parseInt(renderYInput.text);

    model.setRenderPosition(Qt.point(renderX, renderY));
  }

  signal removeClicked()

  Column {
    spacing: 5

    Grid {
      columns: 2
      spacing: 5

      TerminalLabel { text: "Positionning" }
      TerminalComboBox {
        id: positioningInput
        model: ["Tile-based", "Floating"]
        currentIndex: objectEditor.model.flloating ? 1 : 0
        onCurrentTextChanged: {
          objectEditor.model.floating = currentText == "Floating"
        }
      }
      TerminalLabel { text: "Render position"; visible: objectEditor.model.floating }
      Row {
        visible: objectEditor.model.floating
        TerminalField { id: renderXInput; text: model.getSpritePosition().x; onTextChanged: objectEditor.renderPositionChanged() }
        TerminalField { id: renderYInput; text: model.getSpritePosition().y; onTextChanged: objectEditor.renderPositionChanged() }
      }

      TerminalLabel { text: "Grid position" }
      Row {
        TerminalField { id: gridXInput; text: model.position.x; onTextChanged: objectEditor.positionChanged() }
        TerminalField { id: gridYInput; text: model.position.y; onTextChanged: objectEditor.positionChanged() }
      }

      TerminalLabel { text: "Script" }
      TerminalComboBox {
        id: scriptInput
        model: scriptController.getScripts(scriptCategory)
        onCurrentTextChanged: {
          objectEditor.model.setScript(currentText);
        }
      }

      TerminalLabel { text: "Sprite" }
      TerminalComboBox {
        id: spriteInput
        model: animationLibrary.getGroups();
        currentIndex: model.indexOf(objectEditor.model.getSpriteName())
        onCurrentTextChanged: {
          objectEditor.model.setSpriteName(currentText);
        }
      }

      TerminalLabel { text: "Animation" }
      TerminalComboBox {
        id: characterAnimationInput
        model: animationLibrary.getAnimationList(spriteInput.currentText);
        currentIndex: model.indexOf(objectEditor.model.getAnimation())
        onCurrentTextChanged: {
          objectEditor.model.setAnimation(currentText);
        }
      }
    }

    Grid {
      id: additionalFields
      columns: 2
      spacing: 5
    }

    TerminalButton {
      Layout.rowSpan: 2
      Layout.alignment: Qt.AlignCenter
      text: "Remove"
      onClicked: removeClicked()
      padding: 20
      height: 40
    }
  } // END Column
}
