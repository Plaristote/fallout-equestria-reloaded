import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import Game 1.0

Item {
  id: root
  //background: UiStyle.Pane {}
  property var animationGroups: animationLibrary.getGroups()
  property alias animationGroup: groupSelect.currentName
  property var animationNames: []
  property alias animationName: animationSelect.currentName

  onAnimationGroupChanged: {
    animationNames = animationLibrary.getAnimationList(animationGroup)
    console.log("animation names:", animationNames)
  }

  onAnimationNameChanged: {
    spriteAnimation.initialize(animationGroup, animationName)
  }

  SpriteAnimation {
    id: spriteAnimation
    frameCount: 0
    frameInterval: 10
  }

  Dialog {
    id: newGroupDialog
    title: "New sprite group"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label {
        text: "name"
      }
      TextField {
        id: newGroupName
      }
    }
    onAccepted: {
      animationGroups.push(newGroupName.text)
      animationGroupsChanged();
    }
  }

  Dialog {
    id: newSpriteDialog
    title: "New Sprite"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label {
        text: "name"
      }
      TextField {
        id: newSpriteName
      }
    }
    onAccepted: {
      animationNames.push(newSpriteName.text);
      animationNamesChanged();
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
    // Animation select
    EditorSelectPanel {
      id: animationSelect
      model: animationNames
      onNewClicked: newSpriteDialog.open()
    }
    // Animation Editor
    Pane {
      id: animationEditor
      background: UiStyle.TerminalPane {}

      Layout.fillHeight: true
      Layout.fillWidth: true

      function save() {
        spriteAnimation.name = nameInput.text;
        spriteAnimation.source = sourceInput.text;
        spriteAnimation.offset = Qt.point(parseInt(xInput.text), parseInt(yInput.text));
        spriteAnimation.clippedRect = Qt.rect(spriteAnimation.offset.x, spriteAnimation.offset.y, parseInt(widthInput.text), parseInt(heightInput.text));
        spriteAnimation.frameCount = parseInt(frameCountInput.text);
        spriteAnimation.frameInterval = parseInt(intervalInput.text);
        spriteAnimation.repeat = repeatInput.checked;
        animationLibrary.setAnimation(animationGroup, animationName, spriteAnimation);
        animationLibrary.save();
        root.animationGroupChanged();
      }

        Flickable {
          contentHeight: animationForm.height
          clip: true
          anchors {
            top: parent.top; left: parent.left; right: parent.right;
            bottom: animationPreviewBis.top;
          }

          Grid {
            property int col1Width: 120
            id: animationForm
            columns: 2
            spacing: 5
            width: parent.width

            Label {
              text: "name"
              color: "green"
              width: col1Width
            }

            TerminalField {
              id: nameInput
              text: spriteAnimation.name
            }

            Label {
              text: "source"
              color: "green"
              width: col1Width
            }

            TerminalField {
              id: sourceInput
              text: spriteAnimation.source
            }

            Label {
              text: "offset"
              color: "green"
              width: col1Width
            }

            Row {
              spacing: 5
              TerminalField { id: xInput; text: spriteAnimation.offset.x }
              TerminalField { id: yInput; text: spriteAnimation.offset.y }
            }

            Label {
              text: "size"
              color: "green"
              width: col1Width
            }

            Row {
              spacing: 5
              TerminalField { id: widthInput; text: spriteAnimation.clippedRect.width }
              TerminalField { id: heightInput; text: spriteAnimation.clippedRect.height }
            }

            Label {
              text: "frame count"
              color: "green"
              width: col1Width
            }

            TerminalField {
              id: frameCountInput
              text: spriteAnimation.frameCount
            }

            Label {
              text: "interval"
              color: "green"
              width: col1Width
            }

            TerminalField {
              id: intervalInput
              text: spriteAnimation.frameInterval
            }

            Label {
              text: "repeat"
              color: "green"
              width: col1Width
            }

            CheckBox {
              id: repeatInput
              checked: spriteAnimation.repeat
              background: Rectangle { color: "transparent"; border.color: "green"; border.width: 1 }
            }
          }
        } // END form flickable

        Rectangle {
          id: animationPreviewBis
          width: parent.width
          height: animationPreview.height
          anchors.bottom: formControls.top
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
                  source: "../" + sourceInput.text
                  height: parseInt(heightInput.text)
                  width: parseInt(widthInput.text)
                  sourceClipRect: Qt.rect(parseInt(xInput.text) + parseInt(widthInput.text) * index, parseInt(yInput.text), parseInt(widthInput.text), parseInt(heightInput.text))
                }
              }
            }
          }
        }

        MenuButton {
          id: formControls
          anchors.bottom: parent.bottom
          anchors.right: parent.right
          text: "Save"
          onClicked: {
            console.log("save clicked");
            animationEditor.save();
        }
      }
    }

  } // END RowLayout
}
