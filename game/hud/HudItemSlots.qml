import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0
import "qrc:/assets/ui" as UiStyle
import "../../ui"

Item {
  property QtObject inventory
  property int focusedItem: 0
  id: itemSlotsPane

  Component.onCompleted: refreshUsableSlots(itemSlotsRepeater)

  signal itemActivated(QtObject activatedItem)

  Connections {
    target: inventory
    function onEquippedItemsChanged() {
      refreshUsableSlots(itemSlotsRepeater);
    }
  }

  function refreshUsableSlots(target) {
    const slotNames   = inventory.slotNames;
    const usableSlots = [];

    for (var i = 0 ; i < slotNames.length ; ++i) {
      if (slotNames[i].startsWith("use-"))
        usableSlots.push(slotNames[i]);
    }
    target.model = [];
    target.model = usableSlots;
  }

  Row {
    anchors.top: parent.top
    anchors.bottom: parent.bottom

    Repeater {
      id: itemSlotsRepeater
      model: itemSlotsPane.inventory.slotNames
      delegate: Button {
        property string slotName: itemSlotsRepeater.model[index]
        property QtObject slotItem: itemSlotsPane.inventory.getEquippedItem(slotName)

        anchors { top: parent.top; bottom: parent.bottom }
        width: 200
        hoverEnabled: true
        background: BorderImage {
          source: "qrc:/assets/ui/itemSlot.png"
          border { left: 4; bottom: 4; right: 4; top: 4 }
        }
        ColorOverlay {
          anchors.fill: parent
          source: background
          visible: parent.hovered
          color: Qt.rgba(155, 155, 155, 0.3)
        }
        Text {
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.margins: 5
          font.family: application.titleFontName
          font.pixelSize: 14
          color: "yellow"
          text: slotItem ? slotItem.objectName : ""
        }
        Text {
          anchors.bottom: parent.bottom
          anchors.right: parent.right
          anchors.margins: 5
          font.family: application.titleFontName
          font.pixelSize: 14
          color: "yellow"
          text: slotItem ? slotItem.getActionPointCost() + " AP" : ""
        }
        ItemIcon {
          anchors.centerIn: parent
          model: slotItem
        }
        onClicked: itemActivated(slotItem)
      } // END Button
    } // END Repeater
  } // END Row
}
