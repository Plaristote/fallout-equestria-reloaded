import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import Game 1.0

UiStyle.CustomDialog {
  id: dialogAddObject
  property QtObject objectGroup
  property string error

  signal groupAdded(QtObject newObject)

  function openWithGroup(group) {
    objectGroup = group;
    open();
    objectNameInput.forceActiveFocus();
  }

  function resetInputs() {
    objectNameInput.text = error = "";
  }

  function validate() {
    error = objectGroup.validateGroupName(objectNameInput.text);
    return error.length === 0;
  }

  title: i18n.t("game-editor.levels.add-group")
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  background: UiStyle.Pane {}
  onOpened: resetInputs();
  Pane {
    background: UiStyle.TerminalPane {}
    width: parent.width
    GridLayout {
      columns: 2
      width: parent.width

      TerminalLabel { text: i18n.t("game-editor.levels.group-name") }
      TerminalField {
        id: objectNameInput
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        onAccepted: dialogAddObject.accept()
      }

      TerminalLabel { text: i18n.t("game-editor.error"); visible: error.length > 0 }
      TerminalLabel { text: error; visible: error.length > 0 }
    }
  }

  onAccepted: {
    var newObject = objectGroup.factory().generateGroup(objectNameInput.text);;

    groupAdded(newObject);
    resetInputs();
  }
}

