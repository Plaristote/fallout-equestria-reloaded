import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents
import Game 1.0

DynamicObjectEditor {
  id: doorwayEditor
  property var directions: ["", "left", "up", "right", "down"]
  readOnlyAnimation: true

  fields: [
    TerminalLabel { text: "Orientation" },
    TerminalComboBox {
      id: orientationInput
      Layout.fillWidth: true
      model: ["none", "left", "up", "right", "bottom"]
      currentIndex: {
        console.log("Updating current index", doorwayEditor.model.orientation);
        directions.indexOf(doorwayEditor.model.orientation)
      }
      onCurrentIndexChanged: {
        console.log("Set orientation on doorway", doorwayEditor.model, directions[currentIndex]);
        doorwayEditor.model.orientation = directions[currentIndex];
      }
    },

    TerminalLabel { text: "Opened" },
    TerminalCheckBox {
      checked: model.opened
      onCheckedChanged: {
        model.opened = checked
      }
    },

    TerminalLabel { text: "Locked" },
    TerminalCheckBox {
      checked: model.locked
      onCheckedChanged: {
        model.locked = checked;
      }
    },

    TerminalLabel { text: "Key name" },
    TerminalField {
      Layout.fillWidth: true
      text: model.keyName
      onTextChanged: {
        model.keyName = text;
      }
    },

    TerminalLabel { text: "Lock level" },
    TerminalField {
      Layout.fillWidth: true
      text: model.lockpickLevel
      onTextChanged: {
        model.lockpickLevel = parseInt(text);
      }
    }
  ]
}
