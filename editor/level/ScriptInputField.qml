import QtQuick 2.12
import QtQuick.Layouts 1.12
import "../../ui"
import ".." as GameEditor

GameEditor.ScriptInputField {
  onValueChanged: model.setScript(newValue)
  hasScript: model.hasScript
}
