import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../ui/dialog"
import "../game" as GameComponents
import Game 1.0

Item {
  id: root
  property alias currentDialog: dialogSelect.currentName
  property alias currentState: stateSelect.currentName

  Character {
    id: playerCharacter
    characterSheet: "testChar"
    Component.onCompleted: statistics.name = "player"
  }

  Character {
    id: npcCharacter
    characterSheet: "testChar"
    Component.onCompleted: statistics.name = "npc"
  }

  CharacterDialogEditor {
    id: controller

    onStateListChanged: {
      entryPointInput.model = stateSelect.model = stateList;
    }
  }

  onCurrentDialogChanged: {
    var entryPointIndex;
    dialogStateEditor.sourceComponent = null;
    controller.load(currentDialog.replace(".json", ""), playerCharacter, npcCharacter);
    currentState = "";
    stateSelect.model = controller.stateList;
    entryPointIndex = controller.stateList.indexOf(controller.entryPoint);
    entryPointInput.model = controller.stateList;
    entryPointInput.currentIndex = entryPointIndex;
  }

  onCurrentStateChanged: {
    if (currentState !== "") {
      controller.loadState(currentState);
      controller.currentOption = "";
      dialogStateEditor.sourceComponent = dialogStateComponent
    }
    else
      dialogStateEditor.sourceComponent = null
  }

  TextPromptDialog {
    id: addDialogDialog
    title: "New dialog"
    anchors.centerIn: parent
    onAccepted: {
      scriptController.newDialog(value);
      dialogSelect.model = scriptController.getDialogs();
    }
  }

  TextPromptDialog {
    id: addAnswerDialog
    title: "New answer"
    anchors.centerIn: parent
    onAccepted: controller.newAnswer(value);
  }

  TextPromptDialog {
    id: newStateDialog
    title: "New dialog State"
    anchors.centerIn: parent
    onAccepted: {
      controller.newState(value);
      currentState = value;
    }
  }

  ConfirmDialog {
    id: removeStateDialog
    text: "Remove dialog State"
    anchors.centerIn: parent
    onAccepted: {
      controller.removeState();
      currentState = "";
      stateSelect.model = controller.stateList
    }
  }

  ConfirmDialog {
    id: removeAnswerDialog
    title: "Remove answer"
    anchors.centerIn: parent
    onAccepted: controller.removeAnswer()
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectPanel {
      id: dialogSelect
      model: scriptController.getDialogs()
      onNewClicked: addDialogDialog.open()
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
          TerminalLabel { text: "Initial state" }
          TerminalComboBox {
            Layout.fillWidth: true
            id: entryPointInput
            model: controller.stateList
            onCurrentTextChanged: controller.entryPoint = currentText
          }
          RowLayout {
            TerminalLabel { text: "Script"; Layout.fillWidth: true }
            TerminalToolButton {
              iconName: "open"
              onClicked: Qt.openUrlExternally(scriptPath + "dialogs/" + currentDialog.replace(".json", ".mjs"))
            }
          }
        }
      }

      EditorSelectPanel {
        id: stateSelect
        onNewClicked: newStateDialog.open()
        Layout.fillHeight: true
      }
    }

    Pane {
      Layout.fillWidth: true
      Layout.fillHeight: true
      background: UiStyle.Pane {}

      Loader {
        id: dialogStateEditor
        anchors { top: parent.top; left: parent.left; right: parent.right; bottom: formControls.top }
      }

      MenuButton {
        id: formControls
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        text: "Save"
        onClicked: { controller.save(currentDialog.replace(".json", "")); }
      }
    }
  }

  Component {
    id: dialogStateComponent
    DialogStateEditor {
      translationFile: `dialogs/${root.currentDialog.replace(".json", "")}`
    }
  }
}
