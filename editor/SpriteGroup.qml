import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs as BiDialog
import "../assets/ui" as UiStyle
import "../ui"

RowLayout {
  id: root
  property string animationGroup
  property alias animationName: animationSelect.currentName
  property var animationNames: []

  Component.onCompleted: {
    animationNames = animationLibrary.getAnimationList(animationGroup);
  }

  TextPromptDialog {
    id: newSpriteDialog
    title: i18n.t("game-editor.sprites.add-animation")
    anchors.centerIn: parent

    function validate() {
      if (value.length === 0 || value === "defaultSource")
        validationError = i18n.t("game-editor.validations.invalid-name");
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
          text: i18n.t("game-editor.sprites.default-source")
        }
        RowLayout {
          TerminalField {
            id: defaultSourceInput
            onTextChanged: animationLibrary.setDefaultSource(animationGroup, text)
            readOnly: true
            Layout.fillWidth: true
            Component.onCompleted: {
              text = animationLibrary.getDefaultSource(animationGroup);
            }
          }
          TerminalButton {
            text: i18n.t("game-editor.pick")
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

  Loader {
    Layout.fillHeight: true
    Layout.fillWidth: true
    sourceComponent: root.animationName !== "" ? spriteFormComponent : null
  }

  Component {
    id: spriteFormComponent
    ColumnLayout {
      SpriteForm {
        id: spriteForm
        Layout.fillWidth: true
        Layout.fillHeight: true

        Component.onCompleted: initialize(root.animationGroup, root.animationName)

        Connections {
          target: root
          function onAnimationNameChanged() {
            spriteForm.initialize(root.animationGroup, root.animationName);
          }
        }
      }

      Row {
        id: formControls
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        MenuButton {
          text: i18n.t("game-editor.remove")
          enabled: animationName
          onClicked: {
            animationLibrary.remove(root.animationGroup, root.animationName);
            animationNames = animationLibrary.getAnimationList(root.animationGroup);
            animationName = "";
          }
        }

        MenuButton {
          text: i18n.t("game-editor.save")
          enabled: spriteForm.model.hasChanged
          onClicked: {
            console.log("save clicked");
            spriteForm.save();
          }
        }
      }
    }
  } // END SpriteForm
}
