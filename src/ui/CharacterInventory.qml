import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../assets/ui/equipment/settings.mjs" as EquipmentStyle
import "../ui"
import "../game" as GameComponents

Pane {
  id: root
  property QtObject character
  property string mode: "standard"
  readonly property bool gameEditorMode: mode === "gameEditor"
  readonly property bool canEditArmor:   mode !== "combat"
  property QtObject selectedObject
  property var additionalControls: []
  property alias title: header.text
  property var slotLayout: EquipmentStyle.layoutFor(character)

  background: UiStyle.Pane {}

  signal closed()

  onSelectedObjectChanged: {
    console.log("Selected object changed maggle");
    slotsView.item.updateSlots();
  }

  onCharacterChanged: character !== null ? itemsView.inventory = character.inventory : null

  DropArea { anchors.fill: parent }

  Text {
    id: header
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.leftMargin: 10
    text: i18n.t("Inventory")
    font.family: application.titleFontName
    font.pointSize: application.titleFont.pointSize
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
      text: i18n.t("Close")
      onClicked: root.closed();
    }
  }

  RowLayout {
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: footer.top }

    GameComponents.InventoryItemsView {
      id: itemsView
      Layout.fillHeight: true
      Layout.preferredWidth: parent.width > 800 ? 400 : 200
      dragZone: root.parent.parent
      selectedObject: root.selectedObject
      onItemSelected: root.selectedObject = selectedItem
      onItemDropped: if (inventory.isEquippedItem(inventoryItem)) {
        inventory.unequipItem(inventoryItem)
      }

      Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: application.consoleFontName
        font.pointSize: application.consoleFont.tinySize
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

            TerminalToolButton {
              iconName: "see"
              text: i18n.t("Inspect")
              Layout.preferredWidth: 150
              onClicked: selectedObject.triggerInteraction(character, "look")
            }

            TerminalToolButton {
              iconName: "use"
              text: i18n.t("use-modes.use")
              Layout.preferredWidth: 150
              visible: selectedObject && selectedObject.useModes.indexOf("use") >= 0
              onClicked: selectedObject.useFromInventory()
            }

            TerminalToolButton {
              iconName: "drop"
              text: i18n.t("Drop")
              Layout.preferredWidth: 150
              onClicked: character.inventory.dropItem(selectedObject)
            }

            TerminalToolButton {
              iconName: "reload"
              text: i18n.t("use-modes.reload")
              Layout.preferredWidth: 150
              visible: selectedObject && selectedObject.maxAmmo > 0 && selectedObject.ammo < selectedObject.maxAmmo
              onClicked: selectedObject.useReload()
            }

            TerminalToolButton {
              iconName: "unload"
              text: i18n.t("use-modes.unload")
              Layout.preferredWidth: 150
              visible: selectedObject && selectedObject.ammo > 0
              onClicked: selectedObject.useUnload()
            }
          }

          Loader {
            Layout.fillWidth: true
            sourceComponent: root.selectedObject ? itemPreviewComponent : null
          }
        }
      }

      Loader {
        id: slotsView
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        sourceComponent: root.character ? (slotLayout === null ? gridInventorySlots : customInventorySlots) : null
      }
    } // END Column
  } // END RowLayout

  Component {
    id: itemPreviewComponent
    InventoryItemPreview {
      background: Item {}
      model: root.selectedObject
    }
  }

  Component {
    id: gridInventorySlots
    GridInventorySlots {
      canEditArmor: root.canEditArmor
      inventory: root.character?.inventory
      selectedObject: root.selectedObject
      dragZone: root
    }
  }

  Component {
    id: customInventorySlots
    CharacterInventorySlots {
      canEditArmor: root.canEditArmor
      inventory: root.character?.inventory
      selectedObject: root.selectedObject
      layout: slotLayout
      dragZone: root
    }
  }

  Connections {
    target: root.character ? root.character.inventory : root
    ignoreUnknownSignals: root.character == null
    function onEquippedItemsChanged() {
      root.selectedObject = null;
      slotsView.item.updateSlots();
    }
  }

  Connections {
    target: root.character
    function onItemDropped(item) {
      if (root.selectedObject === item)
        root.selectedObject = null;
    }
  }
}
