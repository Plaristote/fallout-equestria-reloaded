import QtQuick 2.15

ItemIcon {
  id: itemIcon
  property var dragZone

  signal clicked(var mouse)

  Drag.active: itemMouseArea.drag.active
  Drag.hotSpot: Qt.point(width / 2, height / 2)
  Drag.keys: ["InventoryItem"]
  Drag.onActiveChanged: {
    if (typeof soundManager != "undefined")
      soundManager.play(Drag.active ? "pick-up" : "put-down");
  }

  Component.onDestruction: {
    if (Drag.active && typeof soundManager != "undefined")
      soundManager.play("put-down");
  }

  states: State {
    when: itemMouseArea.drag.active
    ParentChange { target: itemIcon; parent: dragZone }
    PropertyChanges { target: itemIcon; z: 3 }
  }

  MouseArea {
    id: itemMouseArea
    anchors.fill: parent
    drag.target: parent
    onReleased: {
      const target = parent.Drag.target;
      target ? target.receiveInventoryItem(model) : null;
    }
    onClicked: itemIcon.clicked(mouse)
  }
}
