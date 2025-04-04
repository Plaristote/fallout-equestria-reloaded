import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../../ui"

ColumnLayout {
  property QtObject questManager
  property QtObject hoveredQuest
  id: root

  signal selected(QtObject quest)

  TerminalLabel {
    text: i18n.t("Quests")
    horizontalAlignment: Qt.AlignHCenter
    Layout.fillWidth: true
  }

  RowLayout {
    Layout.fillWidth: true

    TerminalCheckBox {
      id: onGoingEnabled
      checked: true
      text: i18n.t("quests.ongoing")
      Layout.fillWidth: true
    }

    TerminalCheckBox {
      id: completedEnabled
      checked: false
      text: i18n.t("quests.completed")
      Layout.fillWidth: true
    }

    TerminalCheckBox {
      id: failedEnabled
      checked: false
      text: i18n.t("quests.failed")
      Layout.fillWidth: true
    }
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
          property QtObject quest: questManager.list[Math.floor(index / contentColumn.columns)]
          property int column: index % contentColumn.columns
          property color questColor: quest.completed? "lightgray" : "green"

          border.width: 1
          border.color: root.hoveredQuest === quest ? "white" : questColor
          color: "transparent"
          visible: !quest.hidden && ((quest.completed && completedEnabled.checked) || (quest.failed && failedEnabled.checked) || (quest.inProgress && onGoingEnabled.checked))
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
