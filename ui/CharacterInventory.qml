import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents

Pane {
  id: root
  property QtObject character
  property var mode: "standard"
  readonly property bool gameEditorMode: mode === "gameEditor"
  readonly property bool canEditArmor:   mode !== "combat"
  property QtObject selectedObject

  background: UiStyle.Pane {}

  signal closed();

  MenuButton {
    id: closeButton
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Close"
    onClicked: root.closed();
  }

  RowLayout {
    anchors { top: parent.top; left: parent.left; right: parent.right; bottom: closeButton.top }

    GameComponents.InventoryItemsView {
      id: itemsView
      inventory: character.inventory
      Layout.fillHeight: true
      Layout.preferredWidth: parent.width > 800 ? 400 : 200
      selectedObject: root.selectedObject
      onItemSelected: root.selectedObject = selectedItem
    }

    Item {
      Layout.fillHeight: true
      Layout.fillWidth: true

      Column {
        anchors.left: parent.left; anchors.right: parent.right
        spacing: 10

        Pane {
          background: UiStyle.TerminalPane {}
          anchors.left: parent.left; anchors.right: parent.right
          anchors.margins: 10
          height: characterPreviewColumn.height + 20

          Column {
            id: characterPreviewColumn
            anchors.left: parent.left; anchors.right: parent.right

            Image {
              anchors.horizontalCenter: parent.horizontalCenter
              source: "../" + character.getSpriteSource()
              sourceClipRect: character.getClippedRect()
              height: character.getClippedRect().height
              width: character.getClippedRect().height
            }
          }
        }

        InventoryItemPreview {
          anchors.left: parent.left; anchors.right: parent.right
          model: root.selectedObject
        }
      } // END Column
    }
  }
}
