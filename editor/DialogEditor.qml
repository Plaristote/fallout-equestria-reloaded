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
  }

  Character {
    id: npcCharacter
    characterSheet: "testChar"
  }

  CharacterDialogEditor {
    id: controller
  }

  onCurrentDialogChanged: {
    dialogStateEditor.sourceComponent = null;
    controller.load(currentDialog.replace(".json", ""), playerCharacter, npcCharacter);
    currentState = "";
    stateSelect.model = controller.stateList;
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

  Dialog {
    id: addDialogDialog
    title: "New dialog"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "name" }
      TextField { id: newDialogNameInput }
    }
    onAccepted: {
      scriptController.newDialog(newDialogNameInput.text);
      dialogSelect.model = scriptController.getDialogs();
      newDialogNameInput.text = "";
    }
  }

  Dialog {
    id: addAnswerDialog
    title: "New answer"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "name" }
      TextField { id: newAnswerNameInput }
    }
    onAccepted: {
      controller.newAnswer(newAnswerNameInput.text);
      newAnswerNameInput.text = "";
    }
  }

  Dialog {
    id: newStateDialog
    title: "New dialog State"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "name" }
      TextField { id: newStateNameInput }
    }
    onAccepted: {
      controller.newState(newStateNameInput.text);
      stateSelect.model = controller.stateList
      newStateNameInput.text = "";
    }
  }

  Dialog {
    id: removeStateDialog
    title: "Remove dialog State"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: i18n.t("are-you-sure") }
    }
    onAccepted: {
      controller.removeState();
      currentState = "";
      stateSelect.model = controller.stateList
    }
  }

  Dialog {
    id: removeAnswerDialog
    title: "Remove answer"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: i18n.t("are-you-sure") }
    }
    onAccepted: {
      controller.removeAnswer();
    }
  }

  Dialog {
    id: translateStateDialog
    title: "Translate State"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "Text" }
      TextField {
        id: translateStateInput
        text: controller.text
      }
    }
    onAccepted: {
      const key = "dialogs." + currentDialog.replace(".json", "") + "." + controller.stateText;
      scriptController.setTranslation(key, translateStateInput.text);
      controller.loadState(controller.stateReference);
    }
  }

  Dialog {
    id: translateAnswerDialog
    title: "Translate answer"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "Text" }
      TextField {
        id: translateAnswerInput
        text: i18n.t("dialogs." + currentDialog.replace(".json", "") + "." + controller.optionText)
      }
    }
    onAccepted: {
      const key = "dialogs." + currentDialog.replace(".json", "") + "." + controller.optionText;
      scriptController.setTranslation(key, translateAnswerInput.text);
      controller.optionsChanged();
    }
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectPanel {
      id: dialogSelect
      model: scriptController.getDialogs()
      onNewClicked: addDialogDialog.open()
    }

    EditorSelectPanel {
      id: stateSelect
      onNewClicked: newStateDialog.open()
    }

    Pane {
      Layout.fillWidth: true
      Layout.fillHeight: true
      background: UiStyle.Pane {}


      Loader {
        id: dialogStateEditor
        anchors.fill: parent
      }
    }
  }

  Component {
    id: dialogStateComponent
    ColumnLayout {
      anchors.fill: parent
      RowLayout {
        Layout.fillWidth: true
        Pane {
          background: UiStyle.TerminalPane {}
          Layout.fillWidth: true
          Layout.fillHeight: true
          GridLayout {
            width: parent.width
            columns: 2
            TerminalLabel {
              Layout.columnSpan: 2
              Layout.alignment: Qt.AlignTop | Qt.AlignLeft
              text: "State"
              font.pointSize: 13
            }

            TerminalLabel { text: "Text reference" }

            RowLayout {
              Layout.fillWidth: true
              TerminalField {
                Layout.fillWidth: true
                id: stateTextInput
                text: controller.stateText
                onTextChanged: { controller.stateText = text }
              }
              TerminalButton {
                height: stateTextInput.height
                text: "Translate"
                onClicked: translateStateDialog.open()
              }
            }

            TerminalLabel { text: "Trigger callback" }
            TerminalField {
              Layout.fillWidth: true
              id: stateHookInput
              text: controller.stateHook
              onTextChanged: { controller.stateHook = text }
            }

            TerminalButton {
              Layout.columnSpan: 2
              Layout.alignment: Qt.AlignRight
              text: "Remove"
              height: 29
              onClicked: removeStateDialog.open()
            }
          }
        } // END state pane
        Pane {
          background: UiStyle.TerminalPane {}
          Layout.fillWidth: true
          Layout.fillHeight: true
          visible: controller.currentOption !== ""
          GridLayout {
            width: parent.width
            columns: 2
            TerminalLabel {
              text: "Answer"
              font.pointSize: 13
            }
            TerminalLabel {
              text: controller.currentOption
              font.pointSize: 11
            }

            TerminalLabel { text: "Text reference" }

            RowLayout {
              Layout.fillWidth: true
              TerminalField {
                Layout.fillWidth: true
                id: optionTextInput
                text: controller.optionText
                onTextChanged: { controller.optionText = text }
              }
              TerminalButton {
                height: optionTextInput.height
                text: "Translate"
                onClicked: translateAnsweDialog.open()
              }
            }

            TerminalLabel { text: "To state" }
            TerminalComboBox {
              id: optionstateInput
              Layout.fillWidth: true
              model: controller.stateList
              onCurrentTextChanged: controller.optionState = currentText
              Connections {
                target: controller
                function onCurrentOptionChanged() {
                  if (optionstateInput.currentText !== controller.optionState) {
                    optionstateInput.currentIndex = controller.stateList.indexOf(controller.optionState);
                  }
                }
              }
            }

            TerminalLabel { text: "Text callback" }
            TerminalField {
              Layout.fillWidth: true
              text: controller.optionTextHook
              onTextChanged: { controller.optionTextHook = text }
            }

            TerminalLabel { text: "Trigger callback" }
            TerminalField {
              Layout.fillWidth: true
              text: controller.optionHook
              onTextChanged: { controller.optionHook = text }
            }

            TerminalLabel { text: "Available callback" }
            TerminalField {
              Layout.fillWidth: true
              text: controller.optionAvailableHook
              onTextChanged: { controller.optionAvailableHook = text }
            }

            TerminalButton {
              Layout.columnSpan: 2
              Layout.alignment: Qt.AlignRight
              text: "Remove"
              height: 29
              onClicked: removeAnswerDialog.open()
            }
          }
        } // END option pane
      }

      DialogTextDisplay {
        text: controller.text
        Layout.fillWidth: true
        Layout.preferredHeight: 150
      }

      DialogAnswersDisplay {
        Layout.fillHeight: true
        Layout.fillWidth: true
        sourceComponent: Column {
          id: answersList
          width: parent.width
          TerminalButton {
            text: "+ Add answer"
            onClicked: addAnswerDialog.open()
          }

          Repeater {
            model: controller.options
            delegate: Button {
              property color myColor: controller.currentOption === controller.options[index] ? "lightblue" : "green"
              text: "> " + controller.getOptionText(controller.options[index])
              font.family: application.consoleFontName
              hoverEnabled: true
              contentItem: Text { color: parent.hovered ? "white" : parent.myColor; text: parent.text; font: parent.font; wrapMode: Text.WordWrap }
              background: Rectangle { color: "transparent" }
              onClicked: controller.currentOption = controller.options[index]
              width: answersList.width - 10
            }
          }
        } // END Column
      } // END DialogAnswersDisplay

      MenuButton {
        id: formControls
        Layout.alignment: Qt.AlignRight
        text: "Save"
        onClicked: { controller.save(currentDialog.replace(".json", "")); }
      }
    } // END ColumnLayout
  }
}
