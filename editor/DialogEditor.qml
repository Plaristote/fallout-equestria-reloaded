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

  UiStyle.CustomDialog {
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

  UiStyle.CustomDialog {
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
                onClicked: translateAnswerDialog.open()
              }
            }

            TerminalLabel { text: "To state" }
            TerminalComboBox {
              id: optionstateInput
              Layout.fillWidth: true
              model: [''].concat(controller.stateList)
              onCurrentTextChanged: controller.optionState = currentText
              Connections {
                target: controller
                function onCurrentOptionChanged() {
                  if (optionstateInput.currentText !== controller.optionState) {
                    optionstateInput.currentIndex = controller.stateList.indexOf(controller.optionState) + 1;
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
            width: parent.width - 15
            height: 20
            text: "+ Add answer"
            onClicked: addAnswerDialog.open()
          }

          Repeater {
            model: controller.options
            delegate: Button {
              property color myColor: {
                controller.currentOption === controller.options[index]
                    ? "lightblue"
                    : (controller.isOptionAvailable(controller.options[index]) ? "green" : "black")
              }
              text: "> " + controller.getOptionText(controller.options[index])
              font.family: application.consoleFontName
              hoverEnabled: true
              contentItem: Text { color: parent.hovered ? "white" : parent.myColor; text: parent.text; font: parent.font; wrapMode: Text.WordWrap }
              background: Rectangle { color: "transparent" }
              onClicked: controller.currentOption = controller.options[index]
              width: answersList.width - 10

              Column {
                visible: controller.currentOption === controller.options[index]
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                TerminalButton {
                  text: "^"
                  visible: index > 0
                  onClicked: controller.moveOptionUp(index)
                }
                TerminalButton {
                  text: "^"
                  visible: index + 1 < controller.options.length
                  rotation: 180
                  onClicked: controller.moveOptionDown(index)
                }
              }
            }
          }
        } // END Column
      } // END DialogAnswersDisplay
    } // END ColumnLayout
  }
}
