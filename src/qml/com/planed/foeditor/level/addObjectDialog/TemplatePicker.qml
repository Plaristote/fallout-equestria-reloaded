import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/com/planed/foengine/qml/com/planed/foengine/ui/style" as UiStyle
import "qrc:/com/planed/foengine/qml/com/planed/foengine/ui"
import Game 1.0

GridLayout {
  id: root
  property QtObject gameController
  property var templateNames: gameObjectTemplates.names
  property string selectedTemplate: ""
  property bool acceptable: selectedTemplate.length > 0 && nameInput.text.length > 0

  anchors.fill: parent
  columns: 2

  function getTemplateNames(filter) {
    var result = [];

    if (filter.length > 0) {
      for (var i = 0 ; i < gameObjectTemplates.names.length ; ++i) {
        if (gameObjectTemplates.names[i].toLowerCase().indexOf(filter.toLowerCase()) >= 0)
          result.push(gameObjectTemplates.names[i]);
      }
    }
    else
      result = gameObjectTemplates.names;
    return result;
  }

  TerminalLabel {
    text: "Filter"
  }

  TerminalField {
    id: filterInput
    placeholderText: "Filter..."
    placeholderTextColor: "gray"
    Layout.fillWidth: true
    onTextChanged: templateNames = getTemplateNames(text)
  }

  Flickable {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    Layout.fillHeight: true
    contentHeight: gridContainer.height
    contentWidth:  width
    clip: true
    GridLayout {
      id: gridContainer
      columns: 2

      Repeater {
        model: templateNames
        delegate: Rectangle {
          property string templateName: templateNames[index];
          border.color: templateName === selectedTemplate ? "white" : "green"
          color: "transparent"
          clip: true

          width: 385
          height: column.height

          SpriteAnimation {
            id: spriteAnimation
          }

          Component.onCompleted: {
            spriteAnimation.initialize(
              gameObjectTemplates.getSpriteGroup(templateName),
              gameObjectTemplates.getSpriteName(templateName)
            );
          }

          ColumnLayout {
            id: column
            width: parent.width
            RowLayout {
              UiStyle.Icon {
                objectType: gameObjectTemplates.getObjectType(templateName)
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
              }
              TerminalLabel {
                text: templateName
                Layout.fillWidth: true
              }
            }

            Rectangle {
              color: "green"
              Layout.preferredHeight: 1
              Layout.fillWidth: true
            }

            RowLayout {
              Image {
                source: fileProtocol + spriteAnimation.source
                Layout.preferredHeight: 70
                Layout.preferredWidth:  70
                fillMode: Image.PreserveAspectFit
                sourceClipRect: spriteAnimation.clippedRect
              }
              GridLayout {
                Layout.fillWidth: true
                columns: 2

                TerminalLabel {
                  text: "Script"
                }
                TerminalLabel {
                  color: "white"
                  Layout.fillWidth: true
                  text: gameObjectTemplates.getScript(templateName)
                }

                TerminalLabel {
                  text: "Character sheet"
                  visible: gameObjectTemplates.getObjectType(templateName) === "Character"
                }
                TerminalLabel {
                  color: "white"
                  Layout.fillWidth: true
                  text: gameObjectTemplates.getCharacterSheet(templateName)
                  visible: gameObjectTemplates.getObjectType(templateName) === "Character"
                }
              }
            }
          }

          MouseArea {
            anchors.fill: parent
            onClicked: selectedTemplate = templateName;
          }
        } // END delegate
      }
    } // END GridLayout
  } // END Flickable
}
