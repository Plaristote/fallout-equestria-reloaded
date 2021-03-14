import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./assets/ui" as UiStyle
import "./ui"

SaveGameUi {
  id: root
  title: i18n.t("Save")

  slots: [
    SavedGameListItem {
      name: i18n.t("New save game")
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
    MenuButton {
      text: i18n.t("Save")
      onClicked: {
        if (root.selectedIndex == 0)
          newSaveDialog.open();
        else
          overwriteDialog.open();
      }
    },
    MenuButton {
      text: i18n.t("Cancel")
      onClicked: application.popView()
    }
  ]

  Dialog {
    id: newSaveDialog
    title: i18n.t("New save")
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel

    Row {
      Text { text: i18n.t("Name") }
      TextField { id: newSaveNameInput }
    }

    onAccepted: {
      gameManager.saveGame(newSaveNameInput.text);
      application.popView();
    }
  }

  Dialog {
    id: overwriteDialog
    title: i18n.t("Are you sure ?")
    anchors.centerIn: parent
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
      gameManager.saveGame(root.savedGameList[root.selectedIndex - 1]);
      application.popView();
    }
  }
}
