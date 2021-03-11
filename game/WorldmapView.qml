import QtQuick 2.15
import QtQuick.Controls 2.15
import "../assets/ui" as UiStyle

Flickable {
  property QtObject controller;
  property alias mouseX: mapMouseArea.mouseX
  property alias mouseY: mapMouseArea.mouseY
  signal mapClicked();

  clip: true
  contentHeight: mapImage.height
  contentWidth:  mapImage.width

  ScrollBar.vertical:   UiStyle.HudScrollbar { orientation: Qt.Vertical }
  ScrollBar.horizontal: UiStyle.HudScrollbar { orientation: Qt.Horizontal }

  Image {
    id: mapImage
    source: "qrc:/assets/worldmap.png"
    height: controller.mapSize.height
    width: controller.mapSize.width

    Column {
      Repeater {
        model: controller.caseCount.height
        delegate: Row {
          property int indexY: index
          Repeater {
            model: controller.caseCount.width
            delegate: Rectangle {
              id: caseRectangle
              property int indexX: index
              height: controller.caseSize.height - 2
              width:  controller.caseSize.width - 2
              border.width: 1
              border.color: "green"
              color: controller.isVisible(indexX, indexY) ? "transparent" : "black"
              Connections {
                target: controller
                function onCaseRevealed(caseX, caseY) {
                  if (caseX === indexX && caseY === indexY)
                    caseRectangle.color = "transparent";
                }
              }
            }
          }
        }
      }
    }

    MouseArea {
      id: mapMouseArea
      anchors.fill: parent
      onClicked: mapClicked()
    }
  }
}
