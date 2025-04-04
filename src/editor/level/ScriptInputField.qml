import QtQuick 2.12
import ".." as GameEditor

GameEditor.ScriptInputField {
  onValueChanged: model.setScript(newValue)
  hasScript: model.hasScript
}
