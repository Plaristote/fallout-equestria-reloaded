import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0
import "qrc:/assets/ui" as UiStyle
import "../../ui"

Item {
  property QtObject inventory
  property QtObject activeItem
  property int focusedItem: 0
  id: itemSlotsPane

  Component.onCompleted: refreshUsableSlots(itemSlotsRepeater)

  signal itemActivated(string activatedSlot)

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
      delegate: BorderImage {
        property string slotName: itemSlotsRepeater.model[index]
        property QtObject slotItem: itemSlotsPane.inventory.getEquippedItem(slotName)
        property bool activated: activeItem === slotItem

        anchors { top: parent.top; bottom: parent.bottom }
        width: 200
        source: "qrc:/assets/ui/itemSlot.png"
        border { left: 4; bottom: 4; right: 4; top: 4 }
        clip: true

        MouseArea {
          id: mouseArea
          anchors.fill: parent
          hoverEnabled: true
          acceptedButtons: Qt.LeftButton | Qt.RightButton
          cursorShape: Qt.PointingHandCursor
          onClicked: {
            switch (mouse.button) {
            case Qt.LeftButton:
              itemActivated(slotName);
              break;
            case Qt.RightButton:
              slotItem.swapUseMode();
              break ;
            }
          }
        }
        ColorOverlay {
          anchors.fill: parent
          source: parent
          visible: mouseArea.containsMouse && !parent.activated
          color: Qt.rgba(155, 155, 155, 0.1)
        }
        ColorOverlay {
          anchors.fill: parent
          source: parent
          visible: parent.activated && !mouseArea.containsMouse
          color: Qt.rgba(155, 155, 155, 0.2)
        }
        ColorOverlay {
          anchors.fill: parent
          source: parent
          visible: parent.activated && mouseArea.containsMouse
          color: Qt.rgba(155, 155, 155, 0.3)
        }

        ItemIcon {
          anchors.centerIn: parent
          model: slotItem
          visible: slotItem && slotItem.icon !== "any.png"
        }

        Loader {
          anchors { top: parent.top; right: parent.right; bottom: parent.bottom }
          anchors.margins: 5
          sourceComponent: slotItem && slotItem.maxAmmo > 0 ? ammoDisplay : null
          width: 10
        }

        Component {
          id: ammoDisplay
          Rectangle {
            color: "black"
            Rectangle {
              color: "white"
              anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
              height: parent.height * (slotItem.ammo / slotItem.maxAmmo);
            }
          }
        }

        Text {
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.margins: 5
          font.family: application.titleFontName
          font.pixelSize: 14
          color: "yellow"
          style: Text.Outline
          styleColor: "black"
          text: slotItem ? i18n.t("items." + slotItem.itemType) : ""
        }

        Text {
          anchors.bottom: parent.bottom
          anchors.right: parent.right
          anchors.margins: 5
          font.family: application.titleFontName
          font.pixelSize: 14
          color: "yellow"
          style: Text.Outline
          styleColor: "black"
          text: slotItem && slotItem.useMode ? slotItem.getActionPointCost() + " " + i18n.t("AP") : ""
        }

        Text {
          anchors.bottom: parent.bottom
          anchors.left: parent.left
          anchors.margins: 5
          font.family: application.titleFontName
          font.pixelSize: 14
          color: "yellow"
          style: Text.Outline
          styleColor: "black"
          text: slotItem ? i18n.t("use-modes." + slotItem.useMode) : ""
        }

        Shortcut {
          sequence: (index + 1).toString()
          onActivated: itemActivated(slotName)
        }
      } // END Button
    } // END Repeater
  } // END Row
}
