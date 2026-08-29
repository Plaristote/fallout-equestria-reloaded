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

  ColumnLayout {
    id: questContent
    spacing: 5
    width: parent.width

    RowLayout {
      Layout.fillWidth: true

      TerminalLabel {
        text: i18n.t(`quests.${quest.name}.title`)
        font.pointSize: application.consoleFont.hugeSize
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
          font.pointSize: application.consoleFont.bigSize / 6 * 7
        }

        Text {
          text: quest.description
          font.pointSize: application.consoleFont.bigSize / 6 * 7
          horizontalAlignment: Text.AlignJustify
          wrapMode: Text.WordWrap
          textFormat: Text.RichText
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
          font.pointSize: application.consoleFont.bigSize / 6 * 7
        }

        Repeater {
          model: questObjectives
          delegate: TerminalLabel {
            property var objective: questObjectives[index]
            property bool success: quest.isObjectiveCompleted(index)
            property bool failed:  quest.isObjectiveFailed(index)
            property var stroke: success || failed
            property string label: stroke ? `<s>${objective.label}</s>` : objective.label

            text: "- " + label
            color: failed === true ? "red" : "white"
            wrapMode: Text.WordWrap
            Layout.preferredWidth: (root.width / 2) - 50
          }
        }

        Item { Layout.fillHeight: true }
      } // OBJECTIVE COLUMN

    }
  }
}
