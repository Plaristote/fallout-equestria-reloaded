import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Game 1.0
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
            Layout.preferredWidth:  caseSize
            Layout.preferredHeight: caseSize

            Rectangle {
              color: obstacleColor
              visible: caseFlag & LevelGrid.BlockCase
              anchors.fill: parent
            }
            Rectangle {
              color: obstacleColor
              visible: caseFlag & LevelGrid.VerticalWall
              width:   caseSize / 5
              height:  caseSize
              anchors.left: parent.right
            }
            Rectangle {
              color: obstacleColor
              visible: caseFlag & LevelGrid.HorizontalWall
              height:  caseSize / 5
              width:   caseSize
              anchors.bottom: parent.bottom
            }
          }
        } // END col
      } // END row
    } // END grid

    Repeater {
      model: levelController.visibleCharacters
      delegate: Rectangle {
        property QtObject character: levelController.visibleCharacters[index];
        property color colorReference: {
          if (character === levelController.player)
            return "white";
          if (!character.isAlive())
            return "transparent";
          if (character.isEnemy(levelController.player))
            return "red";
          else if (character.isAlly(levelController.player))
            return "lightgreen";
          return "green";
        }
        x: caseSize * character.position.x
        y: caseSize * character.position.y
        width:  caseSize
        height: caseSize
        radius: width * 0.5
        color: colorReference
        SequentialAnimation on color {
          loops: Animation.Infinite
          ColorAnimation { to: "black"; duration: 1000 }
          ColorAnimation { to: colorReference; duration: 100 }
        }

      }
    }
  }
}
