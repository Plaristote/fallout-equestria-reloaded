import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../ui"

Item {
  id: root
  property QtObject questManager
  property QtObject selectedQuest: null
  property var questObjectives: []

  anchors.fill: parent

  onSelectedQuestChanged: {
    questListView.visible = selectedQuest === null
    questView.visible     = selectedQuest !== null
    if (selectedQuest)
      questObjectives = selectedQuest.getObjectives();
  }

  Flickable {
    id: questListView
    contentWidth:  contentColumn.width
    contentHeight: contentColumn.height
    anchors.fill: parent

    Column {
      id: contentColumn
      spacing: 5

      TerminalLabel {
        text: "Quests"
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Item { height: 10 }

      Repeater {
        model: questManager.list
        delegate: Rectangle {
          property QtObject quest: questManager.list[index]
          width: root.width - 20
          height: 20
          border.width: 1
          border.color: quest.completed ? "lightgray" : "green"
          color: "transparent"

          TerminalLabel {
            anchors.centerIn: parent
            text: quest.name
            color: quest.completed? "lightgray" : "green"
          }

          MouseArea {
            anchors.fill: parent
            onClicked: selectedQuest = quest
          }
        }
      }
    }
  } // END Flickable

  Flickable {
    id: questView
    contentWidth: questContent.width
    contentHeight: questContent.height
    anchors.fill: parent
    visible: false

    Column {
      id: questContent
      spacing: 5

      RowLayout {
        TerminalButton {
          text: "Back"
          onClicked: selectedQuest = null
        }

        TerminalLabel {
          text: selectedQuest.name
          Layout.fillWidth: true
        }
      }

      Repeater {
        model: questObjectives
        delegate: TerminalLabel {
          property var objective: questObjectives[index]
          text: objective.label
          color: objective.failed === true ? "red" : "white"

          Rectangle {
            width: parent.width
            color: parent.color
            height: 2
            anchors.verticalCenter: parent.verticalCenter
            visible: objective.success === true || objective.failed === true
          }
        }
      }
    }
  }
}
