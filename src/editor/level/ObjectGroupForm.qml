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

  TerminalLabel { text: `> ${i18n.t("game-editor.levels.object-group")}`; Layout.columnSpan: 3; font.pointSize: 13; }
  TerminalLabel { text: i18n.t("game-editor.levels.group-name") }
  TerminalField {
    Layout.columnSpan: 2
    Layout.fillWidth: true
    text: currentGroup.name
    onTextChanged: if (currentGroup.name !== text) { currentGroup.name = text }
  }

  TerminalLabel { text: i18n.t("game-editor.levels.offset") }
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

  TerminalLabel { text: i18n.t("game-editor.levels.floor"); visible: withFloor }
  TerminalField {
    id: groupFloorInput
    Layout.fillWidth: true
    Layout.columnSpan: 2
    visible: withFloor
    onTextChanged: {
      if (text != currentGroup.floor)
        currentGroup.floor = parseInt(text)
    }
  }

  TerminalLabel { text: i18n.t("game-editor.script"); visible: !readOnlyScript }
  ScriptInputField {
    Layout.fillWidth: true
    Layout.columnSpan: 2
    model: currentGroup
  }
}
