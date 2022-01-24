import QtQuick 2.15

ItemIcon {
  id: itemIcon
  property var dragZone

  signal clicked(var mouse)

  Drag.active: itemMouseArea.drag.active
  Drag.hotSpot: Qt.point(width / 2, height / 2)
  Drag.keys: ["InventoryItem"]

  states: State {
    when: itemMouseArea.drag.active
    ParentChange { target: itemIcon; parent: dragZone }
  }

  MouseArea {
    id: itemMouseArea
    anchors.fill: parent
    drag.target: parent
    onReleased: parent.Drag.target.receiveInventoryItem(model);
    onClicked: itemIcon.clicked(mouse)
  }
}
