import QtQuick 2.15
import QtQuick.Controls 2.15

ScrollBar {
  readonly property int barWidth: 28
  id: scrollBar

  width:  orientation == Qt.Horizontal ? implicitWidth  : barWidth
  height: orientation == Qt.Vertical   ? implicitHeight : barWidth

  background: BorderImage {
    id: verticalBackground
    source: "qrc:/assets/ui/scrollbars/background.png"
    border { left: 6; top: 6; right: 6; bottom: 6 }
    anchors.fill: scrollBar
  }

  contentItem: BorderImage {
    id: verticalContentItem
    source: "qrc:/assets/ui/scrollbars/content.png"
    border { left: 10; top: 15; right: 10; bottom: 20 }
    verticalTileMode: BorderImage.Repeat
    //width: 28
  }
}
