import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../ui"
import Game 1.0

Pane {
  property alias model: spriteAnimation
  property alias name: nameInput.text
  id: animationEditor
  background: UiStyle.TerminalPane {}

  function initialize(animationGroup, animationName) {
    model.initialize(animationGroup, animationName);
    name = animationName;
  }

  function save() {
    const newName = spriteAnimation.name;
    animationLibrary.setAnimation(animationGroup, animationName, spriteAnimation);
    animationLibrary.save();
    root.animationGroupChanged();
    animationName = newName;
  }

  SpriteAnimation {
    id: spriteAnimation
    frameCount: 0
    frameInterval: 10
  }

  Flickable {
    contentHeight: animationForm.height
    clip: true
    anchors {
      top: parent.top; left: parent.left; right: parent.right;
      bottom: animationPreviewBis.top;
    }

    GridLayout {
      property int col1Width: 120
      id: animationForm
      columns: 2
      width: parent.width

      TerminalLabel {
        text: i18n.t("game-editor.sprites.name")
      }

      TerminalField {
        id: nameInput
        Layout.fillWidth: true
        onTextChanged: spriteAnimation.name = text;
      }

      TerminalLabel {
        text: i18n.t("game-editor.sprites.source")
      }

      RowLayout {
        Layout.fillWidth: true

        TerminalField {
          id: sourceInput
          text: spriteAnimation.relativeSource
          Layout.fillWidth: true
          onTextChanged: spriteAnimation.source = text;
        }

        TerminalToolButton {
          iconName: "open"
          onClicked: Qt.openUrlExternally(assetPath + 'sprites/' + spriteAnimation.relativeSource)
        }

        TerminalButton {
          text: i18n.t("game-editor.pick")
          onClicked: {
            filePicker.target = sourceInput;
            filePicker.open();
          }
        }
      }

      TerminalLabel {
        text: i18n.t("game-editor.sprites.offset")
      }

      Row {
        spacing: 5
        TerminalField {
          id: xInput; text: spriteAnimation.offset.x; width: 100
          onTextChanged: spriteAnimation.offset = Qt.point(xInput.text, spriteAnimation.offset.y)
        }
        TerminalField {
          id: yInput; text: spriteAnimation.offset.y; width: 100
          onTextChanged: spriteAnimation.offset = Qt.point(spriteAnimation.offset.x, yInput.text)
        }
      }

      TerminalLabel {
        text: i18n.t("game-editor.sprites.size")
      }

      Row {
        spacing: 5
        TerminalField {
          id: widthInput;  text: spriteAnimation.clippedRect.width;  width: 100
          onTextChanged: spriteAnimation.clippedRect = Qt.rect(xInput.text, yInput.text, widthInput.text, spriteAnimation.clippedRect.height)

        }
        TerminalField {
          id: heightInput; text: spriteAnimation.clippedRect.height; width: 100
          onTextChanged: spriteAnimation.clippedRect = Qt.rect(xInput.text, yInput.text, spriteAnimation.clippedRect.width, heightInput.text)
        }
        TerminalLabel {
          color: "gray"
          text: i18n.t("game-editor.sprites.source-size")
        }
        TerminalLabel {
          color: "lightgray"
          text: `${imageData.sourceSize.width} / ${imageData.sourceSize.height}`
        }
      }

      TerminalLabel {
        text: i18n.t("game-editor.sprites.frame-count")
      }

      TerminalField {
        id: frameCountInput
        text: spriteAnimation.frameCount
        Layout.preferredWidth: 205
        onTextChanged: spriteAnimation.frameCount = parseInt(text)
      }

      TerminalLabel {
        text: i18n.t("game-editor.sprites.interval")
      }

      TerminalField {
        id: intervalInput
        text: spriteAnimation.frameInterval
        onTextChanged: spriteAnimation.frameInterval = parseInt(text)
        Layout.preferredWidth: 205
      }

      TerminalLabel {
        text: i18n.t("game-editor.sprites.repeat")
      }

      TerminalCheckBox {
        id: repeatInput
        checked: spriteAnimation.repeat
        onCheckedChanged: spriteAnimation.repeat = checked
      }

      TerminalLabel {
        text: i18n.t("game-editor.sprites.reverse")
      }

      TerminalCheckBox {
        id: reverseInput
        checked: spriteAnimation.reverse
        onCheckedChanged: spriteAnimation.reverse = checked
      }
    }
  } // END form flickable

  Image {
    id: imageData
    source: assetPath + "sprites/" + sourceInput.text
    visible: false
    onSourceSizeChanged: {
      if (sourceSize.width === 0 || sourceSize.height === 0)
        return ;
      if (spriteAnimation.clippedRect.height === 0 && spriteAnimation.clippedRect.width === 0) {
        widthInput.text  = sourceSize.width;
        heightInput.text = sourceSize.height;
      }
    }
  }

  Rectangle {
    property int previewFrame: 0
    id: animationLoopPreview
    width: parseInt(widthInput.text)
    height: parseInt(heightInput.text)
    anchors.bottom: animationPreviewBis.top
    anchors.bottomMargin: 10
    border.color: "green"
    border.width: 2
    color: "transparent"
    visible: spriteAnimation.frameCount > 1
    Image {
      source: assetPath + "sprites/" + sourceInput.text
      height: parseInt(heightInput.text)
      width: parseInt(widthInput.text)
      sourceClipRect: spriteAnimation.rectForFrame(animationLoopPreview.previewFrame)
      Timer {
        interval: parseInt(intervalInput.text)
        running: animationName != ""
        repeat: true
        onTriggered: {
          animationLoopPreview.previewFrame = animationLoopPreview.previewFrame + 1 >= parseInt(frameCountInput.text) ? 0 : animationLoopPreview.previewFrame + 1;
        }
      }
    }
  }

  Rectangle {
    id: animationPreviewBis
    clip: true
    width: parent.width
    height: animationPreview.height
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 10
    border.color: "green"
    border.width: 2
    color: "transparent"
    Flickable {
      contentWidth: animationPreview.height
      anchors.fill: parent
      Row {
        id: animationPreview
        Repeater {
          model: parseInt(frameCountInput.text)
          delegate: Image {
            source: assetPath + "sprites/" + sourceInput.text
            height: parseInt(heightInput.text)
            width: parseInt(widthInput.text)
            sourceClipRect: {
              const x = parseInt(xInput.text); const width  = parseInt(widthInput.text);
              const y = parseInt(yInput.text); const height = parseInt(heightInput.text);
              const count = parseInt(frameCountInput.text);
              const reversed = reverseInput.checked;
              const frameX = x + (reversed ? width * (count - 1) - (width * index) : width * index);

              return Qt.rect(frameX, y, width, height);
            }
          }
        }
      }
    }
  }
}
