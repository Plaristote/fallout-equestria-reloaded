import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "qrc:/assets/ui" as UiStyle
import "../../ui"

Item {
  property QtObject levelController
  property point player: levelController.player.position
  property var size: levelController.grid.getSize()
  property int caseSize: 15
  property color obstacleColor: "green"
  property color playerColor: "white"

  anchors.fill: parent
  CustomFlickable {
    id: flickable
    anchors.fill: parent
    contentHeight: automapView.height + 15
    contentWidth:  automapView.width  + 15
    ScrollBar.vertical:   UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
    ScrollBar.horizontal: UiStyle.TerminalScrollbar { orientation: Qt.Horizontal }
    contentX: (player.x * caseSize) - width
    contentY: (player.y * caseSize) - height

    Component.onCompleted: centerOn(player)

    GridLayout {
      id: automapView
      columns: size.width
      rows:    size.height
      rowSpacing:    0
      columnSpacing: 0

      Repeater {
        model: size.height
        delegate: Repeater {
          property int yIndex: index
          model: size.width
          delegate: Item {
            property int xIndex: index
            property int caseFlag: levelController.grid.getCaseFlags(xIndex, yIndex)
            property bool isPlayerPosition: player.x === xIndex && player.y === yIndex
            Layout.preferredWidth:  caseSize
            Layout.preferredHeight: caseSize

            Rectangle {
              color: obstacleColor
              visible: caseFlag & 1
              anchors.fill: parent
            }
            Rectangle {
              color: obstacleColor
              visible: caseFlag & 2
              width:   caseSize / 5
              height:  caseSize
              anchors.left: parent.left
            }
            Rectangle {
              color: obstacleColor
              visible: caseFlag & 4
              height:  caseSize / 5
              width:   caseSize
              anchors.bottom: parent.bottom
            }
            Rectangle {
              color: playerColor
              visible: isPlayerPosition
              anchors.fill: parent
            }
          }
        } // END col
      } // END row
    } // END grid
  }
}
