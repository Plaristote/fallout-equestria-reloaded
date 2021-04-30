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
    text: i18n.t("Inventory")
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

    ColumnLayout {
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignTop

      Pane {
        background: UiStyle.TerminalPane {}
        visible: selectedObject != null
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        RowLayout {
          anchors { left: parent.left; right: parent.right }

          ColumnLayout {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: 150

            TerminalButton {
              text: i18n.t("Inspect")
              Layout.preferredWidth: 150
              onClicked: selectedObject.triggerInteraction(character, "look")
            }

            TerminalButton {
              text: i18n.t("Use")
              Layout.preferredWidth: 150
              onClicked: selectedObject.useFromInventory()
            }

            TerminalButton {
              text: i18n.t("Drop")
              Layout.preferredWidth: 150
              onClicked: character.inventory.dropItem(selectedObject)
            }
          }

          InventoryItemPreview {
            background: Item {}
            Layout.fillWidth: true
            model: root.selectedObject
          }
        }
      }

      GridLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        columns: Math.min(2, parent.width / 125)

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
  } // END RowLayout
}
