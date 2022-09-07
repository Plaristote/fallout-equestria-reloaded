import QtQuick 2.15
import "qrc:/game/worldmap"

Column {
  id: root
  property QtObject controller
  property QtObject zone

  Repeater {
    model: controller.caseCount.height
    delegate: Row {
      property int indexY_: index
      Repeater {
        model: controller.caseCount.width
        delegate: WorldmapCase {
          id: caseRectangle
          indexX: index
          indexY: indexY_
          color: getColor()
          controller: root.controller

          function getColor() {
            if (zone.containsCase(indexX, indexY))
              return Qt.rgba(0, 0, 255, 0.5);
            return "transparent";
          }

          Connections {
            target: zone
            function onCasesChanged() {
              caseRectangle.color = caseRectangle.getColor();
            }
          }
        }
      }
    }
  }
}
