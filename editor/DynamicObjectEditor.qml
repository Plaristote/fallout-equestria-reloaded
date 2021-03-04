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
  property bool readOnlyPositionType: false
  property bool readOnlyAnimation: false
  id: objectEditor
  background: UiStyle.TerminalPane {}

  function setTilePosition(tileX, tileY) {
    if (!model.floating) {
      gridXInput.text = tileX;
      gridYInput.text = tileY;
    }
  }

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

  onModelChanged: {
    const posMode        = model.floating ? 1 : 0;
    const spriteName     = model.getSpriteName();
    const animationName  = model.getAnimation();
    const position       = model.getPosition();
    const renderPosition = model.getSpritePosition();

    readOnlyPositionType = model.getObjectType() === "Character";
    readOnlyAnimation    = model.getObjectType() === "Character";
    spriteInput.currentIndex      = spriteInput.model.indexOf(spriteName);
    animationInput.currentIndex   = animationInput.model.indexOf(animationName);
    positioningInput.currentIndex = posMode
    gridXInput.text   = position.x;
    gridYInput.text   = position.y;
    renderXInput.text = renderPosition.x;
    renderYInput.text = renderPosition.y;
  }

  Column {
    spacing: 5

    Grid {
      columns: 2
      spacing: 5

      TerminalLabel { text: "Positionning"; visible: !objectEditor.readOnlyPositionType }
      TerminalComboBox {
        id: positioningInput
        visible: !objectEditor.readOnlyPositionType
        model: ["Tile-based", "Floating"]
        currentIndex: objectEditor.model.floating ? 1 : 0
        onCurrentTextChanged: {
          objectEditor.model.floating = currentText == "Floating"
        }
      }
      TerminalLabel { text: "Render position"; visible: objectEditor.model.floating }
      Row {
        visible: objectEditor.model.floating
        TerminalField { id: renderXInput; onTextChanged: objectEditor.renderPositionChanged() }
        TerminalField { id: renderYInput; onTextChanged: objectEditor.renderPositionChanged() }
      }

      TerminalLabel { text: "Grid position" }
      Row {
        TerminalField { id: gridXInput; onTextChanged: objectEditor.positionChanged() }
        TerminalField { id: gridYInput; onTextChanged: objectEditor.positionChanged() }
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
          if (objectEditor.model.getObjectType() === "Character")
            objectEditor.model.setAnimation("idle");
          else
            objectEditor.model.setAnimation(objectEditor.model.getAnimation())
        }
      }

      TerminalLabel { text: "Animation"; visible: !readOnlyAnimation }
      TerminalComboBox {
        id: animationInput
        visible: !readOnlyAnimation
        model: animationLibrary.getAnimationList(spriteInput.currentText);
        currentIndex: model.indexOf(objectEditor.model.getAnimation())
        onCurrentTextChanged: {
          if (!readOnlyAnimation)
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
