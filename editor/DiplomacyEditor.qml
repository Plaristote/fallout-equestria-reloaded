import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Item {
  id: root
  property QtObject dataEngine
  property alias currentFaction: factionSelect.currentName

  Connections {
    target: dataEngine
    function onDiplomacyUpdated() {
      console.log("DiplomacyEditor: onDiplomacyUpdated");
      factionSelect.model = dataEngine.getFactionList();
      updateFactionView();
    }
  }

  onCurrentFactionChanged: updateFactionView()

  function updateFactionView() {
    if (currentFaction.length > 0) {
      console.log("refresh enemy list for", currentFaction);
      enemyRepeater.model = dataEngine.getEnemyListForFaction(currentFaction);
      reputationCheckbox.checked = dataEngine.hasFactionReputationEnabled(currentFaction);
    }
    else {
      enemyRepeater.model = []
    }
  }

  Dialog {
    id: newFactionDialog
    title: "New faction"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "name" }
      TextField { id: newFactionNameInput }
    }
    onAccepted: {
      dataEngine.registerFaction(newFactionNameInput.text);
    }
  }

  Dialog {
    id: newEnemyDialog
    title: "Add enemy"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    Row {
      Label { text: "name" }
      ComboBox {
        id: newEnemySelect
        model: factionSelect.model
      }
    }
    onAccepted: {
      dataEngine.setFactionAsEnemy(currentFaction, newEnemySelect.currentText);
    }
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectPanel {
      id: factionSelect
      model: dataEngine.getFactionList()
      onNewClicked: newFactionDialog.open()
    }

    Pane {
      background: UiStyle.Pane {}
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.rightMargin: 50
      Layout.bottomMargin: saveButton.height

      ColumnLayout {
        anchors.fill: parent

        Pane {
          id: descriptionPane
          background: UiStyle.PaperPane {}
          Layout.fillWidth: true

          Column {
            width: parent.width
            spacing: 10

            Text {
              width: parent.width
              id: descriptionTitle
              text: currentFaction
              font.family: application.titleFontName
              font.pointSize: 15
              font.letterSpacing: 2
              wrapMode: Text.WordWrap
            }

            Flickable {
              width: parent.width
              contentHeight: descriptionContent.height
              clip: true
              height: 200
              Text {
                width: parent.width
                id: descriptionContent
                text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In risus tellus, commodo vitae pretium at, viverra elementum velit. Phasellus et finibus magna. Praesent fermentum, tortor eu vehicula hendrerit, magna sem iaculis metus, vitae placerat quam ligula vitae nibh. In accumsan sodales tortor, nec efficitur tortor bibendum in. In hac habitasse platea dictumst. Vivamus pellentesque ex sit amet mauris fringilla porta. Phasellus mollis enim nec lacus tempus, sit amet pretium nulla vestibulum. Suspendisse non tortor quis tortor auctor posuere nec a ligula. Suspendisse a lectus sollicitudin, feugiat dolor consectetur, tempus ligula. Etiam at diam at quam rutrum fringilla. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam facilisis sodales eros eget viverra."
                wrapMode: Text.WordWrap
                font.family: application.consoleFontName
                font.pointSize: 8
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
                      onClicked: dataEngine.setFactionAsEnemy(currentFaction, factionName, false)
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
  }

  MenuButton {
    id: saveButton
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Save"
    onClicked: dataEngine.saveToFile("./assets/game.json")
  }
}
