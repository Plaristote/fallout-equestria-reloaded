import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Item {
  property QtObject gameController;
  property QtObject levelController;
  anchors.fill: parent

  LevelCanvas {
    id: canvas
    levelController: parent.levelController
  }

  ScreenEdges {
    onMoveTop:    { canvas.translate(0, scrollSpeed); }
    onMoveLeft:   { canvas.translate(scrollSpeed, 0); }
    onMoveRight:  { canvas.translate(-scrollSpeed, 0); }
    onMoveBottom: { canvas.translate(0, -scrollSpeed); }
  }

  Pane {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    height: 135
    width: Math.min(parent.width, 1025)
    background: UiStyle.PlayPanel {}
  }
}
