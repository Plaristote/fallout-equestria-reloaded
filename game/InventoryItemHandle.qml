import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/ui"
import "qrc:/assets/ui" as UiStyle

Rectangle {
  id: itemRectangle
  property QtObject inventoryItem
  property int itemCount
  property bool selected: false
  property var dragZone
  property string dragType: "InventoryItem"
  property int itemIconWidth: 72
  property int itemIconHeight: 72

  signal clicked()
  signal doubleClicked()

  width: Math.max(itemIconWidth, itemIconHeight)
  height: width
  border.width: selected ? 1 : 0
  border.color: "green"
  radius: 5
  color: itemMouseArea.drag.active ? "black" : "transparent"
  Drag.active: itemMouseArea.drag.active
  Drag.hotSpot.x: width / 2
  Drag.hotSpot.y: height / 2
  Drag.keys: ["InventoryItem"]
  ItemIcon {
    anchors.centerIn: parent
    model: inventoryItem
    width: itemIconWidth
    height: itemIconHeight
  }
  states: State {
    when: itemMouseArea.drag.active
    ParentChange { target: itemRectangle; parent: itemRectangle.dragZone }
  }
  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 5
    text: itemCount > 1 ? `x${itemCount}` : ""
    font.family: application.consoleFontName
    font.pointSize: 8
    style: Text.Raised
    styleColor: "black"
    color: "white"
    width: itemIconWidth
    wrapMode: Text.WrapAnywhere
  }
  MouseArea {
    id: itemMouseArea
    anchors.fill: parent
    drag.target: parent
    hoverEnabled: true
    onPressed: itemRectangle.clicked()
    onDoubleClicked: itemRectangle.doubleClicked()
    onReleased: {
      console.log("coucou", parent.Drag.target);
      if (parent.Drag.target && parent.Drag.target.receiveInventoryItem)
        parent.Drag.target.receiveInventoryItem(inventoryItem);
    }
  }
}
