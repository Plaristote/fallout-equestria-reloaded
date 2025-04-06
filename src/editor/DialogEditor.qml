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
      const newEntryPointIndex = stateList.indexOf(entryPointInput.currentText);

      entryPointInput.model = stateSelect.treeModel = stateList;
      entryPointInput.currentIndex = newEntryPointIndex;
    }
  }

  onCurrentDialogChanged: {
    var entryPointIndex;
    dialogStateEditor.sourceComponent = null;
    controller.load(currentDialog.replace(".json", ""), playerCharacter, npcCharacter);
    currentState = "";
    stateSelect.treeModel = controller.stateList;
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

  TreeTextPromptDialog {
    id: addDialogDialog
    tree: dialogSelect
    title: i18n.t("game-editor.dialogs.add")
    anchors.centerIn: parent
    onAccepted: {
      scriptController.newDialog(path);
      dialogSelect.treeModel = scriptController.getDialogs();
    }
  }

  TextPromptDialog {
    id: addAnswerDialog
    title: i18n.t("game-editor.dialogs.answers.add")
    anchors.centerIn: parent
    onAccepted: controller.newAnswer(value);
  }

  TreeTextPromptDialog {
    id: newStateDialog
    tree: stateSelect
    title: i18n.t("game-editor.dialogs.states.add")
    anchors.centerIn: parent
    onAccepted: {
      controller.newState(path);
      currentState = path;
    }
  }

  ConfirmDialog {
    id: removeStateDialog
    text: i18n.t("game-editor.dialogs.states.remove")
    anchors.centerIn: parent
    onAccepted: {
      controller.removeState();
      currentState = "";
      stateSelect.treeModel = controller.stateList
    }
  }

  ConfirmDialog {
    id: removeAnswerDialog
    title: i18n.t("game-editor.dialogs.answers.remove")
    anchors.centerIn: parent
    onAccepted: controller.removeAnswer()
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectTree {
      id: dialogSelect
      treeModel: scriptController.getDialogs()
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
          TerminalLabel { text: i18n.t("game-editor.dialogs.states.initial") }
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
          TerminalLabel { text: i18n.t("options.locale") }
          TerminalComboBox {
            Layout.fillWidth: true
            model: i18n.getAvailableLocales()
            onCurrentIndexChanged: {
              const newLocale = i18n.getAvailableLocales()[currentIndex];
              if (i18n.currentLocale !== newLocale)
                i18n.currentLocale = newLocale;
            }
          }
        }
      }

      EditorSelectTree {
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
        text: i18n.t("game-editor.save")
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
