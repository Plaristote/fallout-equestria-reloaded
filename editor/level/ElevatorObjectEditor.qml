import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents
import Game 1.0

DynamicObjectEditor {
  id: elevatorEditor
  Component.onCompleted: {
    if (model)
      updateModel();
  }

  Connections {
    target: elevatorEditor
    function onModelChanged() { updateModel(); }
  }

  function updateModel() {
    const posA = { x: model.positionA.x, y: model.positionA.y, z: model.floorA };
    const posB = { x: model.positionB.x, y: model.positionB.y, z: model.floorB };

    console.log(`posA{${posA.x}, ${posA.y}, ${posA.z}}`);
    console.log(`posB{${posB.x}, ${posB.y}, ${posB.z}}`);
    posAXInput.text  = posA.x;
    posAYInput.text  = posA.y;
    floorAInput.text = posA.z;
    posBXInput.text  = posB.x;
    posBYInput.text  = posB.y;
    floorBInput.text = posB.z;
  }

  function elevatorPositionChanged() {
    model.setPositionA(parseInt(posAXInput.text), parseInt(posAYInput.text), parseInt(floorAInput.text));
    model.setPositionB(parseInt(posBXInput.text), parseInt(posBYInput.text), parseInt(floorBInput.text));
  }

  fields: [
    TerminalLabel { text: "Position A" },
    Row {
      Layout.fillWidth: true
      visible: objectEditor.model.floating
      TerminalField { id: posAXInput; onTextChanged: elevatorEditor.elevatorPositionChanged(); width: parent.width / 2 }
      TerminalField { id: posAYInput; onTextChanged: elevatorEditor.elevatorPositionChanged(); width: parent.width / 2 }
    },

    TerminalLabel { text: "Floor A" },
    TerminalField { id: floorAInput; onTextChanged: elevatorEditor.elevatorPositionChanged(); },

    TerminalLabel { text: "Position B" },
    Row {
      Layout.fillWidth: true
      visible: objectEditor.model.floating
      TerminalField { id: posBXInput; onTextChanged: elevatorEditor.elevatorPositionChanged(); width: parent.width / 2 }
      TerminalField { id: posBYInput; onTextChanged: elevatorEditor.elevatorPositionChanged(); width: parent.width / 2 }
    },

    TerminalLabel { text: "Floor B" },
    TerminalField { id: floorBInput; onTextChanged: elevatorEditor.elevatorPositionChanged(); }
  ]
}
