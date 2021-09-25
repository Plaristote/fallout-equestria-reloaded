import QtQuick 2.12
import QtQuick.Layouts 2.21
import "../../ui"

GridLayout {
  property QtObject currentGroup

  columns: 3

  onCurrentGroupChanged: {
    const offset = currentGroup.offset;
    groupOffsetXInput.text = offset.x;
    groupOffsetYInput.text = offset.y;
  }

  TerminalLabel { text: "> Group"; Layout.columnSpan: 3; font.pointSize: 13; }
  TerminalLabel { text: "Name" }
  TerminalField {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: currentGroup.name
    onTextChanged: if (currentGroup.name !== text) { currentGroup.name = text }
  }

  TerminalLabel { text: "Position" }
  TerminalField {
    id: groupOffsetXInput
    Layout.fillWidth: true
    onTextChanged: currentGroup.offset = Qt.point(parseInt(groupOffsetXInput.text), parseInt(groupOffsetYInput.text))
  }
  TerminalField {
    id: groupOffsetYInput
    Layout.fillWidth: true
    onTextChanged: currentGroup.offset = Qt.point(parseInt(groupOffsetXInput.text), parseInt(groupOffsetYInput.text))
  }
}
