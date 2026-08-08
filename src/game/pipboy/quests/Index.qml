import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../../ui"

ColumnLayout {
  property QtObject questManager
  property QtObject hoveredQuest
  property var questList: sortQuestList(questManager.list);
  id: root

  signal selected(QtObject quest)

  function sortQuestList(list) {
    return list.sort(function(a, b) {
      if (a.inProgress && !b.inProgress)
        return -1;
      else if (b.inProgress && !a.inProgress)
        return 1;
      return 0;
    });
  }

  TerminalLabel {
    text: i18n.t("Quests")
    horizontalAlignment: Qt.AlignHCenter
    Layout.fillWidth: true
  }

  Rectangle { color: "lightgreen"; Layout.fillWidth: true; implicitHeight: 1 }

  Flickable {
    contentHeight: contentColumn.height
    Layout.fillHeight: true
    Layout.fillWidth: true

    GridLayout {
      id: contentColumn
      rowSpacing: 1
      columnSpacing: 5
      columns: 3
      anchors { left: parent.left; right: parent.right }

      Repeater {
        model: questManager.list.length * contentColumn.columns
        delegate: Rectangle {
          property QtObject quest: root.questList[Math.floor(index / contentColumn.columns)]
          property int column: index % contentColumn.columns
          property color questColor: quest.completed ? "lightgray" : "green"

          border.width: 1
          border.color: root.hoveredQuest === quest ? "white" : questColor
          color: "transparent"
          visible: !quest.hidden
          implicitHeight: visible ? 30 : 0
          implicitWidth: cellContent.width + 20
          Layout.fillWidth: column === 0

          Loader {
            id: cellContent
            anchors.centerIn: parent
            sourceComponent: {
              switch (column) {
              case 0: return questColumn1;
              case 1: return questColumn2;
              case 2: return questColumn3;
              }
            }
          }

          Component {
            id: questColumn1
            TerminalLabel {
              text: i18n.t(`quests.${quest.name}.title`)
              color: questColor
            }
          }

          Component {
            id: questColumn2
            TerminalLabel {
              text: i18n.t(`locations.${quest.location}`)
              color: questColor
            }
          }

          Component {
            id: questColumn3
            TerminalLabel {
              text: `${quest.completeCount} / ${quest.objectiveCount}`
              color: questColor
            }
          }

          MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: selected(quest)
            onHoveredChanged: root.hoveredQuest = hovered ? quest : null
          }
        }
      }
    }
  }
}
