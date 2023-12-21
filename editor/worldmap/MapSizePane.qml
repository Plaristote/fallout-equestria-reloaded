import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../ui"
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject model

  background: UiStyle.TerminalPane {}
  Column {
    TerminalLabel {
      anchors.horizontalCenter: parent.horizontalCenter
      text: i18n.t("game-editor.worldmap.pixel-size")
    }

    Grid {
      columns: 3
      TerminalLabel {
        text: i18n.t("game-editor.worldmap.map-size")
      }
      TerminalField {
        id: mapSizeWidthInput
        text: model.mapSize.width
        onTextChanged: model.mapSize.width = parseInt(text)
      }
      TerminalField {
        id: mapSizeHeightInput
        text: model.mapSize.height
        onTextChanged: model.mapSize.height = parseInt(text)
      }
      TerminalLabel {
        text: i18n.t("game-editor.worldmap.case-size")
      }
      TerminalField {
        id: caseWidthInput
        text: model.caseSize.width
        onTextChanged: model.caseSize.width = parseInt(text)
      }
      TerminalField {
        id: caseHeightInput
        text: model.caseSize.height
        onTextChanged: model.caseSize.height = parseInt(text)
      }
    }
  }
} // END Map Sizing Panel
