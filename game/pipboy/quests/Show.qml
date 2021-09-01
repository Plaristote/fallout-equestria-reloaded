import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../../ui"

Flickable {
  property QtObject quest
  property var questObjectives: quest.getObjectives()
  id: root

  signal exited()

  contentHeight: questContent.height
  anchors.fill: parent
  visible: false

  ColumnLayout {
    id: questContent
    spacing: 5
    anchors { left: parent.left; right: parent.right }

    RowLayout {
      Layout.fillWidth: true

      TerminalLabel {
        text: i18n.t(`quests.${quest.name}.title`)
        font.pointSize: 16
        Layout.fillWidth: true
      }

      TerminalButton {
        text: i18n.t("Close")
        onClicked: root.exited()
      }
    }

    Rectangle {
      color: "green"
      implicitHeight: 1
      Layout.fillWidth: true
    }

    RowLayout {
      Layout.fillWidth: true
      Layout.fillHeight: true

      ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        TerminalLabel {
          text: i18n.t("quests.description")
          font.pointSize: 14
        }

        Text {
          text: quest.description
          font.pointSize: 14
          horizontalAlignment: Text.AlignJustify
          wrapMode: Text.WordWrap
          Layout.preferredWidth: (root.width / 2) - 50
          color: "green"
        }

        Item { Layout.fillHeight: true }
      }

      ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        TerminalLabel {
          text: i18n.t("quests.objectives")
          font.pointSize: 14
        }

        Repeater {
          model: questObjectives
          delegate: TerminalLabel {
            property var objective: questObjectives[index]
            text: "- " + objective.label
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

        Item { Layout.fillHeight: true }
      } // OBJECTIVE COLUMN

    }
  }
}
