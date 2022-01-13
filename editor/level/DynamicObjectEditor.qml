import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents
import Game 1.0

ColumnLayout {
  spacing: 5
  property QtObject gameController
  property QtObject model
  property string scriptCategory: "behaviour"
  property alias fields: additionalFields.children
  property bool readOnlyPositionType: false
  property bool readOnlySprite: false
  property bool readOnlyAnimation: false
  property bool readOnlyScript: false
  property bool withFloor: gameController.level.floorCount > 1
  property bool withOrientation: false
  property bool withPathBlocking: false
  property bool withCover: false
  property var directions: ["", "left", "up", "right", "down"]
  id: objectEditor

  signal requestSpriteView(string group)

  function setTilePosition(tileX, tileY, floor) {
    gridXInput.text = tileX;
    gridYInput.text = tileY;
    floorInput.text = floor;
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

    model.spriteOffset = Qt.point(renderX, renderY);
    //model.setRenderPosition(Qt.point(renderX, renderY));
  }

  onModelChanged: {
    const posMode             = model.floating ? 1 : 0;
    const spriteName          = model.spriteName;
    const animationName       = model.getAnimation();
    const position            = { x: model.position.x,     y: model.position.y };
    const renderPosition      = { x: model.spriteOffset.x, y: model.spriteOffset.y };
    const animationList       = animationLibrary.getAnimationList(model.spriteName);

    animationInput.currentIndex   = animationList.indexOf(animationName);
    positioningInput.currentIndex = posMode
    gridXInput.text          = position.x;
    gridYInput.text          = position.y;
    renderXInput.text        = renderPosition.x;
    renderYInput.text        = renderPosition.y;
    nameInput.text           = model.objectName;
    floorInput.text          = model.floor;
    pathBlockInput.checked   = model.blocksPath;
    coverInput.text          = model.cover.toString();
  }

  GridLayout {
    columns: 2
    Layout.fillWidth: true

      TerminalLabel { text: "Name" }
      TerminalField {
        id: nameInput
        onTextChanged: if (objectEditor.model.objectName !== text) { objectEditor.model.objectName = text; }
        Layout.fillWidth: true
      }

      TerminalLabel { text: "Positionning"; visible: !objectEditor.readOnlyPositionType }
      TerminalComboBox {
        Layout.fillWidth: true
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
        Layout.fillWidth: true
        visible: objectEditor.model.floating
        TerminalField { id: renderXInput; onTextChanged: objectEditor.renderPositionChanged(); width: parent.width / 2 }
        TerminalField { id: renderYInput; onTextChanged: objectEditor.renderPositionChanged(); width: parent.width / 2 }
      }

      TerminalLabel { text: "Grid position" }
      Row {
        Layout.fillWidth: true
        TerminalField { id: gridXInput; onTextChanged: objectEditor.positionChanged(); width: parent.width / 2 }
        TerminalField { id: gridYInput; onTextChanged: objectEditor.positionChanged(); width: parent.width / 2 }
      }

      TerminalLabel { text: "Floor"; visible: withFloor }
      TerminalField {
        id: floorInput
        visible: withFloor
        Layout.fillWidth: true
        onTextChanged: {
          if (text != objectEditor.model.floor)
            objectEditor.model.floor = Math.min(parseInt(text), gameController.level.floorCount - 1);
        }
      }

      TerminalLabel { text: "Script"; visible: !readOnlyScript }
      ScriptInputField {
        Layout.fillWidth: true
        scriptCategory: objectEditor.scriptCategory
        model: objectEditor.model
        visible: !readOnlyScript
      }

      TerminalLabel { text: "Sprite"; visible: !readOnlySprite }
      SpriteInputField {
        Layout.fillWidth: true
        visible: !readOnlySprite
        model: objectEditor.model
        onRequestSpriteView: root.requestSpriteView(group)
      }

      TerminalLabel { text: "Animation"; visible: !readOnlyAnimation }
      TerminalComboBox {
        Layout.fillWidth: true
        id: animationInput
        visible: !readOnlyAnimation
        model: animationLibrary.getAnimationList(objectEditor.model.spriteName);
        currentIndex: model.indexOf(objectEditor.model.getAnimation())
        onCurrentTextChanged: {
          if (!readOnlyAnimation)
            objectEditor.model.setAnimation(currentText);
        }
      }

      TerminalLabel { text: "Orientation"; visible: withOrientation }
      TerminalComboBox {
        id: orientationInput
        Layout.fillWidth: true
        visible: withOrientation
        model: ["none", "left", "up", "right", "bottom"]
        currentIndex: {
          console.log("Updating current index", objectEditor.model.orientation);
          directions.indexOf(objectEditor.model.orientation)
        }
        onCurrentIndexChanged: {
          console.log("Set orientation on doorway", objectEditor.model, directions[currentIndex]);
          objectEditor.model.orientation = directions[currentIndex];
        }
      }

      TerminalLabel { text: "Blocks path"; visible: withPathBlocking }
      TerminalCheckBox {
        id: pathBlockInput
        visible: withPathBlocking
        Layout.fillWidth: true
        onCheckedChanged: objectEditor.model.blocksPath = checked
      }

      TerminalLabel { text: "Cover"; visible: withCover }
      TerminalField {
        id: coverInput
        visible: withCover
        Layout.fillWidth: true
        onTextChanged: objectEditor.model.cover = Math.max(0, Math.min(parseInt(text), 100))
      }
    }

  TerminalLabel { text: "> Behaviour"; font.pointSize: 13 }

  GridLayout {
    id: additionalFields
    width: parent.width
    columns: 2
  }
}
