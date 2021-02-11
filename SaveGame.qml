import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./assets/ui" as UiStyle
import "./ui"

SaveGameUi {
  id: root
  title: "Save"

  slots: [
    SavedGameListItem {
      name: "New save game"
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
      text: "Save"
      onClicked: {
        if (root.selectedIndex == 0)
          newSaveDialog.open();
        else
          overwriteDialog.open();
      }
    },
    MenuButton {
      text: "Cancel"
      onClicked: application.popView()
    }
  ]

  Dialog {
    id: newSaveDialog
    title: "New save"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel

    Row {
      Text { text: "Name" }
      TextField { id: newSaveNameInput }
    }

    onAccepted: {
      gameManager.saveGame(newSaveNameInput.text);
      application.popView();
    }
  }

  Dialog {
    id: overwriteDialog
    title: "Are you sure ?"
    anchors.centerIn: parent
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
      gameManager.saveGame(root.savedGameList[root.selectedIndex]);
      application.popView();
    }
  }
}
