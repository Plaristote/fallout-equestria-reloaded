import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./assets/ui" as UiStyle
import "./ui"

SaveGameUi {
  id: root
  title: i18n.t("Save")
  currentSave: selectedIndex > 0 ? savedGameList[selectedIndex - 1] : null

  Action {
    id: saveAction
    text: i18n.t("Save")
    shortcut: Shortcut {
      sequences: ["Enter", "Return"]
      onActivated: saveAction.trigger()
    }
    onTriggered: {
      if (root.selectedIndex == 0)
        newSaveDialog.open();
      else
        overwriteDialog.open();
    }
  }

  Action {
    id: saveGame
    onTriggered: {
      gameManager.saveGame(root.savedGameList[root.selectedIndex - 1]);
      application.popView();
    }
  }

  slots: [
    SavedGameListItem {
      name: i18n.t("New save")
      isNewSlot: true
      selected: root.selectedIndex == 0
      onClicked: root.selectedIndex = 0
      onDoubleClicked: { root.selectedIndex = 0; newSaveDialog.open(); }
      Layout.fillWidth: true
    },
    Repeater {
      model: root.savedGameList
      delegate: SavedGameListItem {
        name: root.savedGameList[index]
        selected: root.selectedIndex == index + 1
        onClicked: root.selectedIndex = index + 1
        onDoubleClicked: { root.selectedIndex = index + 1; overwriteDialog.open(); }
        Layout.fillWidth: true
      }
    }
  ]

  controls: [
    MenuButton { action: saveAction },
    MenuButton {
      text: i18n.t("Cancel")
      onClicked: application.popView()
    }
  ]

  ConfirmDialog {
    id: newSaveDialog
    title: i18n.t("New save")
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    onVisibleChanged: if (visible) { newSaveNameInput.forceActiveFocus(); }

    UiStyle.TerminalPane {
      width: parent.width
      height: 75
      RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        TerminalLabel { text: i18n.t("Name") }
        TerminalField {
          id: newSaveNameInput
          Layout.fillWidth: true
          onAccepted: newSaveDialog.accept()
        }
      }
    }

    onAccepted: {
      gameManager.saveGame(newSaveNameInput.text);
      application.popView();
    }
  }

  ConfirmDialog {
    id: overwriteDialog
    title: i18n.t("Are you sure ?")
    anchors.centerIn: parent
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: saveGame.trigger()
    onVisibleChanged: if (visible) { virtualInput.forceActiveFocus(); }
    TerminalField {
      id: virtualInput
      visible: false
      onAccepted: overwriteDialog.accept()
    }
  }
}
