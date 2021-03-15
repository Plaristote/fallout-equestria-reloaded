import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

BorderImage {
  property alias sourceComponent: answersList.sourceComponent

  id: answersPane
  source: "qrc:/assets/ui/dialog/center.png"
  border { top: 41; left: 80; right: 58; bottom: 31 }
  horizontalTileMode: BorderImage.Stretch
  verticalTileMode: BorderImage.Stretch

  Flickable {
    id: answersFlickable
    clip: true
    contentHeight: answersList.height
    anchors.fill: parent
    anchors.leftMargin: 80
    anchors.rightMargin: 68
    anchors.topMargin: 51
    anchors.bottomMargin: 41

    ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

    Loader {
      id: answersList
      width: parent.width
    }
  }
}
