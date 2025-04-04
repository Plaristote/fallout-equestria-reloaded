import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import "qrc:/ui"

Image {
  property var menuEntries: [continueAction, newGameAction, loadGameAction, optionsAction, creditsAction, exitAction]
  source: assetPath + "backgrounds/default.png"
  fillMode: Image.PreserveAspectCrop

  Item {
      width: 300
      height: 300
      anchors.top: parent.top
      anchors.right: parent.right

    Image {
      id: leimage
      source: assetPath + "backgrounds/default.png"
      fillMode: Image.PreserveAspectCrop
      anchors.fill: parent
      visible: false
    }

    ShaderEffect {
      fragmentShader: "qrc:/game/level/PlayerCropCircle.shader.frag.qsb"
      vertexShader: "qrc:/game/level/PlayerCropCircle.shader.vert.qsb"
      anchors.fill: parent
      property alias source: leimage
      property alias src: leimage
      property vector2d centerPoint: Qt.vector2d(width/2, height/2)
      property real radius: Math.min(width, height) / 4
      property real smoothEdge: 10.0

      mesh: GridMesh {
        resolution: Qt.size(10, 10)
      }
    }
  }

  Component.onCompleted: {
    if (developmentEdition)
      menuEntries = menuEntries.slice(0, 4).concat([gameEditorAction, creditsAction, exitAction]);
  }

  Action {
    id: continueAction
    text: i18n.t("Continue")
    enabled: gameManager.hasContinueGame()
    onTriggered: application.gameLoading = gameManager.getSavedGames()[0]
  }

  Action {
    id: newGameAction
    text: i18n.t("New game")
    onTriggered: application.createGame()
  }

  Action {
    id: loadGameAction
    text: i18n.t("Load game")
    enabled: gameManager.getSavedGames().length > 0
    onTriggered: application.pushView("LoadGame.qml")
  }

  Action {
    id: optionsAction
    text: i18n.t("Options")
    onTriggered: application.pushView("Options.qml")
  }

  Action {
    id: creditsAction
    text: i18n.t("Credits")
    onTriggered: application.pushView("Credits.qml")
  }

  Action {
    id: exitAction
    text: i18n.t("Exit")
    onTriggered: application.close();
  }

  Action {
    id: gameEditorAction
    text: i18n.t("Game Editor")
    onTriggered: application.pushView("editor/Index.qml")
  }

  ColumnLayout {
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.margins: 50
    spacing: 10

    Repeater {
      model: menuEntries
      delegate: MenuButton {
        Layout.fillWidth: true
        action: modelData
      }
    }
  }
}
