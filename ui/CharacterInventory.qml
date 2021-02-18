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
  property var additionalControls: []
  property alias title: header.text

  background: UiStyle.Pane {}

  signal closed()

  onSelectedObjectChanged: {
    console.log("Selected object changed maggle");
    slotsView.updateSlots();
  }

  onCharacterChanged: character !== null ? itemsView.inventory = character.inventory : null

  Text {
    id: header
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.leftMargin: 10
    text: "Inventory"
    font.family: application.titleFontName
    font.pointSize: 18
    color: "white"
  }

  Row {
    id: footer
    anchors.bottom: parent.bottom
    anchors.right: parent.right

    Repeater {
      model: additionalControls
      delegate: MenuButton {
        text:      additionalControls[index].label
        onClicked: { additionalControls[index].callback() }
      }
    }

    MenuButton {
      id: closeButton
      text: "Close"
      onClicked: root.closed();
    }
  }

  RowLayout {
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: footer.top }

    GameComponents.InventoryItemsView {
      id: itemsView
      Layout.fillHeight: true
      Layout.preferredWidth: parent.width > 800 ? 400 : 200
      selectedObject: root.selectedObject
      onItemSelected: root.selectedObject = selectedItem

      Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: application.consoleFontName
        font.pointSize: 8
        color: "green"
        text: {
          if (character === null)
            return "Character is null"
          if (itemsView.inventory === null)
            return "Inventory is null";
          return itemsView.inventory.totalWeight + ' / ' + character.statistics.carryWeight;
        }
      }
    }

    Item {
      Layout.fillHeight: true
      Layout.fillWidth: true
      clip: true

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
              source: fileProtocol + character.getSpriteSource()
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

        GridLayout {
          columns: Math.min(2, parent.width / 125)
          anchors.horizontalCenter: parent.horizontalCenter

          CharacterInventorySlots {
            id: slotsView
            canEditArmor: root.canEditArmor
            inventory: root.character.inventory
            selectedObject: root.selectedObject
          }
        }
      } // END Column

      Connections {
        target: root.character.inventory
        function onEquippedItemsChanged() {
          root.selectedObject = null;
          slotsView.updateSlots();
        }
      } // END Inventory Connections
    } // END Item
  } // END RowLayout
}
