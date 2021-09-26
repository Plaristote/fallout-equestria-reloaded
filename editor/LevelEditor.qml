import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import "./level" as LevelEditorUi
import Game 1.0

Item {
  id: root
  property var gameController
  property alias currentLevelName: levelSelect.currentName

  signal requestCharacterView(string characterSheet)
  signal requestSpriteView(string group)

  onCurrentLevelNameChanged: {
    console.log("LevelEditor::onCurrentLevelNameChanged")
    gameController.switchToLevel(currentLevelName, "");
  }

  Connections {
    target: gameController

    function onLevelChanged() {
      levelEditorUiLoader.sourceComponent = null;
      if (gameController.level)
        levelEditorUiLoader.sourceComponent = levelEditorUi;
    }

    function onLoadError(error) {
      errorDialog.text = error;
      errorDialog.open()
    }
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectPanel {
      id: levelSelect
      model: scriptController.getLevels()
      readOnly: true
    }

    Loader {
      id: levelEditorUiLoader
      Layout.fillHeight: true
      Layout.fillWidth: true
    }
  }

  MenuButton {
    id: saveButton
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Save"
    onClicked: {
      gameController.save();
      gameController.getDataEngine().saveToFile("./assets/game.json");
    }
  }

  Component {
    id: levelEditorUi
    LevelEditorUi.Editor {
      gameController: root.gameController
      onRequestCharacterView: root.requestCharacterView(characterSheet)
      onRequestSpriteView: root.requestSpriteView(group)
    }
  }

  UiStyle.CustomDialog {
    property alias text: errorMessage.text
    id: errorDialog
    modal: true
    parent: Overlay.overlay
    standardButtons: Dialog.Ok
    anchors.centerIn: parent

    Label {
      id: errorMessage
      color: "white"
      font.family: application.titleFontName
      font.pixelSize: 16
    }
  }
}
