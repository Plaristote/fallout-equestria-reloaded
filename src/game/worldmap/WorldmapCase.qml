import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../assets/ui" as UiStyle
import "../../ui"

Rectangle {
  id: caseRectangle
  property int indexX
  property int indexY
  property QtObject controller

  height: controller?.caseSize?.height
  width:  controller?.caseSize?.width
  border.width: 1
  border.color: "green"
  color: controller && controller.isVisible(indexX, indexY) ? "transparent" : "black"
  Connections {
    target: controller
    function onCaseRevealed(caseX, caseY) {
      if (caseX === indexX && caseY === indexY)
        caseRectangle.color = "transparent";
    }
  }
}
