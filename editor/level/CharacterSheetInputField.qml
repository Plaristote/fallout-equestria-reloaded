import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../ui"
import "." as LevelEditor

RowLayout {
  property var model

  TerminalButton {
    id: sheetInput
    Layout.fillWidth: true
    text: model.characterSheet
    onClicked: sheetPicker.startPicking(model.characterSheet + ".json")
    LevelEditor.CharacterSheetPicker {
      id: sheetPicker
      parent: Overlay.overlay
      anchors.centerIn: parent
      onAccepted: model.characterSheet = pickedOption.replace(".json", "")
    }
  }

  TerminalToolButton {
    iconName: "open"
    onClicked: characterEditor.requestCharacterView(model.characterSheet)
  }
}
