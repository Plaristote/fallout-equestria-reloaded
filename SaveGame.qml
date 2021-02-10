import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./assets/ui" as UiStyle
import "./ui"

Pane {
  id: root
  property var savedGameList: gameManager.getSavedGames()
  property var selectedIndex: 0
  background: UiStyle.Pane {}

  Text {
    id: viewTitle
    text: "Save"
    color: "white"
    font.pointSize: 25
    anchors.topMargin: 10
    anchors.leftMargin: 10
    anchors.top: parent.top
    anchors.left: parent.left
  }

  Pane {
    id: slotListPane
    background: UiStyle.TerminalPane {}
    anchors.bottom: controls.top;
    anchors.left: parent.left
    anchors.top: viewTitle.bottom
    width: parent.width * 0.7

    Flickable {
      contentHeight: slotList.height
      anchors.fill: parent

      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

      ColumnLayout {
        id: slotList
        width: parent.width - 15

        SavedGameListItem {
          name: "New save game"
          isNewSlot: true
          selected: root.selectedIndex == 0
          onClicked: root.selectedIndex = 0
          onDoubleClicked: { root.selectedIndex = 0; newSaveDialog.open(); }
          Layout.fillWidth: true
        }

        Repeater {
          model: savedGameList
          delegate: SavedGameListItem {
            name: savedGameList[index]
            selected: root.selectedIndex == index + 1
            onClicked: root.selectedIndex = index + 1
            onDoubleClicked: { root.selectedIndex = index + 1; overwriteDialog.open(); }
            Layout.fillWidth: true
          }
        }
      }
    }
  }

  Pane {
    id: savePreview
    background: UiStyle.Pane {}
    anchors.bottom: controls.top
    anchors.left: slotListPane.right
    anchors.top: viewTitle.bottom
    anchors.right: parent.right
  }

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

  Row {
    id: controls
    anchors { bottom: parent.bottom; right: parent.right; }

    MenuButton {
      text: "Save"
      onClicked: {
        if (root.selectedIndex == 0)
          newSaveDialog.open();
        else
          overwriteDialog.open();
      }
    }

    MenuButton {
      text: "Cancel"
      onClicked: application.popView()
    }
  }
}
