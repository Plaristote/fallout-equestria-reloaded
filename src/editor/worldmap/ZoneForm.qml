import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

Pane {
  property QtObject zoneModel

  signal nameChanged();
  signal previousClicked();
  signal destroyClicked();

  id: zonePanel
  background: UiStyle.TerminalPane {}
  clip: true

  Column {
    anchors.fill: parent
    spacing: 5

    TerminalButton {
      text: `< ${i18n.t("game-editor.previous")}`
      onClicked: previousClicked()
      height: 20
      width: parent.width
    }

    GridLayout {
      width: parent.width
      columns: 2

      TerminalLabel {
        text: i18n.t("game-editor.worldmap.zones.name")
      }

      TerminalField {
        Layout.fillWidth: true
        text: zoneModel.name
        onTextChanged: {
          zoneModel.name = text;
          nameChanged();
        }
      }

      TerminalLabel {
        text: i18n.t("game-editor.worldmap.zones.movement-speed")
      }

      TerminalField {
        Layout.fillWidth: true
        text: zoneModel.movementSpeed
        onTextChanged: zoneModel.movementSpeed = parseInt(text)
      }
    }

    TerminalButton {
      text: i18n.t("game-editor.worldmap.zones.remove")
      onClicked: destroyClicked()
      width: parent.width
      height: 25
    }
  }
}
