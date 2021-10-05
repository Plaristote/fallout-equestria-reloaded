import QtQuick 2.12
import QtQuick.Layouts 1.12
import "../../ui"

GridLayout {
  property QtObject currentGroup
  property bool withFloor: false

  columns: 3

  onCurrentGroupChanged: {
    const offset = currentGroup.offset;
    groupOffsetXInput.text = offset.x;
    groupOffsetYInput.text = offset.y;
    groupFloorInput.text   = currentGroup.floor;
  }

  TerminalLabel { text: "> Group"; Layout.columnSpan: 3; font.pointSize: 13; }
  TerminalLabel { text: "Name" }
  TerminalField {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: currentGroup.name
    onTextChanged: if (currentGroup.name !== text) { currentGroup.name = text }
  }

  TerminalLabel { text: "Offset" }
  TerminalField {
    id: groupOffsetXInput
    Layout.fillWidth: true
    onTextChanged: {
      if (parseInt(text) !== currentGroup.offset.x)
        currentGroup.offset = Qt.point(parseInt(groupOffsetXInput.text), parseInt(groupOffsetYInput.text));
    }
  }
  TerminalField {
    id: groupOffsetYInput
    Layout.fillWidth: true
    onTextChanged: {
      if (parseInt(text) !== currentGroup.offset.y)
        currentGroup.offset = Qt.point(parseInt(groupOffsetXInput.text), parseInt(groupOffsetYInput.text))
    }
  }

  TerminalLabel { text: "Floor"; visible: withFloor }
  TerminalField {
    id: groupFloorInput
    Layout.fillWidth: true
    Layout.columnSpan: 2
    onTextChanged: {
      if (text != currentGroup.floor)
        currentGroup.floor = parseInt(text)
    }
  }

  TerminalLabel { text: "Script"; visible: !readOnlyScript }
  ScriptInputField {
    Layout.fillWidth: true
    Layout.columnSpan: 2
    model: currentGroup
  }
}
