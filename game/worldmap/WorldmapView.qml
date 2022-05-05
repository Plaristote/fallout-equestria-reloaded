import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../assets/ui" as UiStyle
import "../../ui"

CustomFlickable {
  property QtObject controller;
  property alias mouseX: mapMouseArea.mouseX
  property alias mouseY: mapMouseArea.mouseY
  property alias content: beforeCases.children
  signal mapClicked();

  clip: true
  contentHeight: mapImage.height + 30
  contentWidth:  mapImage.width + 30

  ScrollBar.vertical:   UiStyle.HudScrollbar { orientation: Qt.Vertical }
  ScrollBar.horizontal: UiStyle.HudScrollbar { orientation: Qt.Horizontal }

  Image {
    id: mapImage
    source: assetPath + "worldmap.png"
    height: controller.mapSize.height
    width: controller.mapSize.width

    Item {
      id: beforeCases
      anchors.fill: parent
    }

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
              height: controller.caseSize.height
              width:  controller.caseSize.width
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
