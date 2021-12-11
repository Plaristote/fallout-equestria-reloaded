import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  property QtObject dataEngine
  property string currentFaction
  background: UiStyle.Pane {}

  Component.onCompleted: refresh()

  function refresh() {
    enemyRepeater.model = dataEngine.getEnemyListForFaction(currentFaction);
    reputationCheckbox.checked = dataEngine.hasFactionReputationEnabled(currentFaction);
  }

  function save() {
    dataEngine.saveToFile("./assets/game.json")
    scriptController.setTranslation(`factions.${currentFaction}.name`, descriptionTitle.text)
    scriptController.setTranslation(`factions.${currentFaction}.description`, descriptionContent.text)
  }

  UiStyle.CustomDialog {
    id: newEnemyDialog
    title: "Add enemy"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    RowLayout {
      anchors.fill: parent
      Label { text: "Name"; color: "white"; font.family: application.titleFontName }
      SelectBox {
        id: newEnemySelect
        model: factionSelect.model
        Layout.fillWidth: true
      }
    }
    onAccepted: {
      dataEngine.setFactionAsEnemy(currentFaction, newEnemySelect.currentText);
      refresh();
    }
  }

  ColumnLayout {
    anchors.fill: parent

    Pane {
      id: descriptionPane
      background: UiStyle.PaperPane {}
      Layout.fillWidth: true

      Column {
        width: parent.width
        spacing: 10

        TextArea {
          width: parent.width
          id: descriptionTitle
          text: i18n.t(`factions.${currentFaction}.name`)
          font.family: application.titleFontName
          font.pointSize: 15
          font.letterSpacing: 2
          wrapMode: Text.WordWrap
          color: "black"
        }

        Flickable {
          width: parent.width
          contentHeight: descriptionContent.height
          clip: true
          height: 200
          TextArea {
            width: parent.width
            id: descriptionContent
            text: i18n.t(`factions.${currentFaction}.description`)
            wrapMode: Text.WordWrap
            font.family: application.consoleFontName
            font.pointSize: 8
            color: "black"
          }
        }
      }
    }

    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillWidth: true
      GridLayout {
        TerminalLabel { text: "Enable reputation" }
        TerminalCheckBox {
          id: reputationCheckbox
          onCheckedChanged: dataEngine.setFactionReputationEnabled(currentFaction, checked)
        }
      }
    }

    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillWidth: true
      Layout.fillHeight: true
      Flickable {
        contentHeight: enemyColumnLayout.height
        anchors.fill: parent
        clip: true

        ColumnLayout {
          id: enemyColumnLayout
          width: parent.width
          TerminalButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            text: "Add enemy"
            onClicked: newEnemyDialog.open()
          }

          Repeater {
            id: enemyRepeater
            delegate: Rectangle {
              property string factionName: enemyRepeater.model[index]
              Layout.fillWidth: true
              Layout.preferredHeight: 30
              border.width: 1
              border.color: "green"
              color: "transparent"
              RowLayout {
                width: parent.width

                TerminalLabel {
                  Layout.alignment: Qt.AlignVCenter
                  text: factionName
                }
                TerminalButton {
                  Layout.alignment: Qt.AlignRight
                  text: "x"
                  onClicked: {
                    dataEngine.setFactionAsEnemy(currentFaction, factionName, false);
                    refresh();
                  }
                  height: 30
                }
              }
            }
          }
        }
      }
    } // ENEMY EDITOR
  } // ColumnLayout
}
