import QtQuick 2.15

Column {
  property QtObject controller
  property QtObject zone

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
          color: getColor()

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
