import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2 as BiDialog
import "qrc:/assets/ui" as UiStyle
import "../ui"
import Game 1.0

Item {
  id: root
  property var animationGroups: animationLibrary.getGroups()
  property alias animationGroup: groupSelect.currentName
  property var animationNames: []
  property alias animationName: animationSelect.currentName

  onAnimationGroupChanged: {
    animationNames = animationLibrary.getAnimationList(animationGroup);
    defaultSourceInput.text = animationLibrary.getDefaultSource(animationGroup);
    animationName = "";
    console.log("animation names:", animationNames)
  }

  onAnimationNameChanged: {
    spriteAnimation.initialize(animationGroup, animationName)
    nameInput.text = animationName;
  }

  SpriteAnimation {
    id: spriteAnimation
    frameCount: 0
    frameInterval: 10
  }

  TextPromptDialog {
    id: newGroupDialog
    title: "New sprite group"
    anchors.centerIn: parent

    onAccepted: {
      animationGroups.push(value)
      animationGroupsChanged();
    }
  }

  TextPromptDialog {
    id: newSpriteDialog
    title: "New Sprite"
    anchors.centerIn: parent

    function validate() {
      if (value.length === 0 || value === "defaultSource")
        validationError = "Invalid or reserved name.";
      else
        validationError = "";
      return validationError === "";
    }

    onAccepted: {
      animationNames.push(value);
      animationNamesChanged();
      animationName = value;
      value = "";
    }
  }

  BiDialog.FileDialog {
    property var target
    id: filePicker
    title: "Please chose a file"
    folder: rootPath + "assets/sprites"
    nameFilters: ["Image files (*.jpg, *.png)"]

    onAccepted: {
      const path = filePicker.fileUrl.toString().replace(/.*\/assets\/sprites\//, "")
      console.log("File selected", path);
      target.text = path;
    }
  }

  RowLayout {
    anchors.fill: parent

    // Groups select
    EditorSelectPanel {
      id: groupSelect
      model: animationGroups
      onNewClicked: newGroupDialog.open()
    }

    ColumnLayout {
      spacing: 5
      Layout.preferredWidth: 200
      Layout.fillHeight: true

      Pane {
        Layout.preferredWidth: 200
        background: UiStyle.TerminalPane {}
        ColumnLayout {
          width: parent.width
          TerminalLabel {
            text: "Default source"
          }
          RowLayout {
            TerminalField {
              id: defaultSourceInput
              onTextChanged: animationLibrary.setDefaultSource(animationGroup, text)
              readOnly: true
              Layout.fillWidth: true
            }
            TerminalButton {
              text: "Pick"
              onClicked: {
                filePicker.target = defaultSourceInput
                filePicker.open()
              }
            }
          }
        }
      }

      // Animation select
      EditorSelectPanel {
        id: animationSelect
        model: animationNames
        onNewClicked: newSpriteDialog.open()
      }
    }

    // Animation Editor
    Pane {
      id: animationEditor
      background: UiStyle.TerminalPane {}

      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.bottomMargin: formControls.height

      function save() {
        const newName = spriteAnimation.name;
        animationLibrary.setAnimation(animationGroup, animationName, spriteAnimation);
        animationLibrary.save();
        root.animationGroupChanged();
        animationName = newName;
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
            text: "name"
          }

          TerminalField {
            id: nameInput
            Layout.fillWidth: true
            onTextChanged: spriteAnimation.name = text;
          }

          TerminalLabel {
            text: "source"
          }

          RowLayout {
            Layout.fillWidth: true

            TerminalField {
              id: sourceInput
              text: spriteAnimation.relativeSource
              Layout.fillWidth: true
              onTextChanged: spriteAnimation.source = text;
            }

            TerminalButton {
              text: "Pick"
              onClicked: {
                filePicker.target = sourceInput;
                filePicker.open();
              }
            }
          }

          TerminalLabel {
            text: "offset"
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
            text: "size"
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
          }

          TerminalLabel {
            text: "frame count"
          }

          TerminalField {
            id: frameCountInput
            text: spriteAnimation.frameCount
            Layout.preferredWidth: 205
            onTextChanged: spriteAnimation.frameCount = parseInt(text)
          }

          TerminalLabel {
            text: "interval"
          }

          TerminalField {
            id: intervalInput
            text: spriteAnimation.frameInterval
            onTextChanged: spriteAnimation.frameInterval = parseInt(text)
            Layout.preferredWidth: 205
          }

          TerminalLabel {
            text: "repeat"
          }

          TerminalCheckBox {
            id: repeatInput
            checked: spriteAnimation.repeat
            onCheckedChanged: spriteAnimation.repeat = checked
          }
        }
      } // END form flickable

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
        Image {
          source: assetPath + "sprites/" + sourceInput.text
          height: parseInt(heightInput.text)
          width: parseInt(widthInput.text)
          sourceClipRect: Qt.rect(parseInt(xInput.text) + parseInt(widthInput.text) * animationLoopPreview.previewFrame, parseInt(yInput.text), parseInt(widthInput.text), parseInt(heightInput.text))
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
                sourceClipRect: Qt.rect(parseInt(xInput.text) + parseInt(widthInput.text) * index, parseInt(yInput.text), parseInt(widthInput.text), parseInt(heightInput.text))
              }
            }
          }
        }
      }
    }
  } // END RowLayout

  Row {
    id: formControls
    anchors.bottom: parent.bottom
    anchors.right: parent.right

    MenuButton {
      text: "Remove"
      enabled: animationName
      onClicked: {
        animationLibrary.remove(animationGroup, animationName);
        animationNames = animationLibrary.getAnimationList(animationGroup);
        animationName = "";
      }
    }

    MenuButton {
      text: "Save"
      enabled: animationName && spriteAnimation.hasChanged
      onClicked: {
        console.log("save clicked");
        animationEditor.save();
      }
    }
  }
}
