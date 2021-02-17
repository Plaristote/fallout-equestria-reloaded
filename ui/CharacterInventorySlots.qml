import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Repeater {
  id: root
  property QtObject inventory
  property QtObject selectedObject

  model: inventory.slotNames

  signal equippedItem()

  function updateSlots() {
    model = [];
    model = inventory.slotNames
  }

  delegate: Pane {
    property string slotName: inventory.slotNames[index]
    property QtObject equippedItem: inventory.getEquippedItem(slotName)
    background: UiStyle.Label {}
    height: 125
    width:  125

    ColumnLayout {
      anchors.fill: parent

      Text {
        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
        text: slotName
        font.family: application.titleFontName
        color: "yellow"
      }

      Image {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        visible:        equippedItem !== null
        source:         equippedItem !== null ? fileProtocol + equippedItem.getSpriteSource() : ""
        sourceClipRect: equippedItem !== null ? equippedItem.getClippedRect() : Qt.rect(0,0,0,0)
        height:         50
        width:          75
        fillMode:       Image.PreserveAspectCrop
      }

      Row {
        Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter

        Button {
          visible: equippedItem !== null
          text: "-"
          height: 20
          width: 70 / 2
          onClicked: {
            inventory.unequipItem(slotName)
            root.equippedItem()
          }
        }

        Button {
          visible: root.selectedObject !== null && inventory.canEquipItem(root.selectedObject, slotName)
          text: "+"
          height: 20
          width: 70 / 2
          onClicked: {
            inventory.equipItem(selectedObject, slotName);
            root.equippedItem();
          }
        }
      }
    }
  } // END Slot
} // END Slots
