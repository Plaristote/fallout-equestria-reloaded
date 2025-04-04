import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../ui"
import "./quests" as QuestApp

Item {
  id: root
  property QtObject questManager
  property QtObject selectedQuest: null

  anchors.fill: parent

  QuestApp.Index {
    id: questListView
    anchors.fill: parent
    questManager: root.questManager
    onSelected: root.selectedQuest = quest
    visible: selectedQuest == null
  }

  Loader {
    anchors.fill: parent
    sourceComponent: selectedQuest != null ? show : null
  }

  Component {
    id: show
    QuestApp.Show {
      quest:    root.selectedQuest
      onExited: root.selectedQuest = null
    }
  }
}
