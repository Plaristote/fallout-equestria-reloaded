import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../ui/dialog"
import "../game" as GameComponents
import Game 1.0

ColumnLayout {
  id: stateLayout
  property string translationFile
  anchors.fill: parent
  RowLayout {
    Layout.fillWidth: true
    Pane {
      background: UiStyle.TerminalPane {}
      Layout.preferredWidth: stateLayout.width / 2
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

        TerminalLabel { text: i18n.t("game-editor.dialogs.states.text") }

        TranslationInputField {
          Layout.fillWidth: true
          id: stateTextInput
          translationKey: controller.stateText
          translationFile: stateLayout.translationFile
          prefix: "dialogs." + currentDialog.replace(".json", "")
          onRequireKeyChange: controller.stateText = newKey
          onUpdated: controller.loadState(controller.stateReference)
        }

        TerminalLabel { text: i18n.t("game-editor.dialogs.states.trigger") }
        TerminalField {
          Layout.fillWidth: true
          id: stateHookInput
          text: controller.stateHook
          onTextChanged: { controller.stateHook = text }
        }

        TerminalLabel { text: i18n.t("game-editor.dialogs.states.mood") }
        TerminalField {
          Layout.fillWidth: true
          id: stateMoodInput
          text: controller.stateMood
          onTextChanged: { controller.stateMood = text }
        }

        TerminalButton {
          Layout.columnSpan: 2
          Layout.alignment: Qt.AlignRight
          text: i18n.t("game-editor.remove")
          height: 29
          onClicked: removeStateDialog.open()
        }
      }
    } // END state pane
    Pane {
      background: UiStyle.TerminalPane {}
      Layout.preferredWidth: stateLayout.width / 2
      Layout.fillHeight: true
      visible: controller.currentOption !== ""
      GridLayout {
        width: parent.width
        columns: 2
        TerminalLabel {
          text: i18n.t("game-editor.dialogs.answer")
          font.pointSize: 13
        }
        TerminalLabel {
          text: controller.currentOption
          font.pointSize: 11
        }

        TerminalLabel { text: i18n.t("game-editor.dialogs.states.text") }

        TranslationInputField {
          Layout.fillWidth: true
          id: optionTextInput
          translationKey: controller.optionText
          translationFile: stateLayout.translationFile
          prefix: "dialogs." + currentDialog.replace(".json", "")
          onRequireKeyChange: controller.optionText = newKey
          onUpdated: controller.optionsChanged()
        }

        TerminalLabel { text: i18n.t("game-editor.dialogs.answers.target") }
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

        TerminalLabel { text: i18n.t("game-editor.dialogs.states.text") }
        TerminalField {
          Layout.fillWidth: true
          text: controller.optionTextHook
          onTextChanged: { controller.optionTextHook = text }
        }

        TerminalLabel { text: i18n.t("game-editor.dialogs.states.trigger") }
        TerminalField {
          Layout.fillWidth: true
          text: controller.optionHook
          onTextChanged: { controller.optionHook = text }
        }

        TerminalLabel { text: i18n.t("game-editor.dialogs.answers.available-callback") }
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
        text: `+ ${i18n.t("game-editor.dialogs.answers.add")}`
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
          onClicked: {
            controller.currentOption = controller.options[index]
            soundManager.play("ui/term-btn-click");
          }
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
