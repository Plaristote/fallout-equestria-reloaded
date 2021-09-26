import QtQuick 2.12
import QtQuick.Layouts 1.12
import "../ui"
import "./level" as LevelEditor

RowLayout {
  id: root
  property string scriptCategory: "behaviour"
  property var model
  property bool hasScript: model.scriptName.length > 0

  signal valueChanged(string newValue)

  TerminalButton {
    id: scriptInput
    Layout.fillWidth: true
    text: root.hasScript ? root.model.scriptName : "N/A"
    onClicked: scriptPicker.openWithCategory(scriptCategory)
    LevelEditor.ScriptPicker {
      id: scriptPicker
      parent: Overlay.overlay
      anchors.centerIn: parent
      onAccepted: {
        valueChanged(pickedOption);
        scriptInput.text = pickedOption;
      }
      onClosed: scriptInput.forceActiveFocus()
    }
  }
  TerminalToolButton {
    iconName: "open"
    onClicked: Qt.openUrlExternally(scriptPath + '/' + scriptCategory + '/' + root.model.scriptName)
  }
}
