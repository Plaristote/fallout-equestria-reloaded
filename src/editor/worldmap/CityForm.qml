import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

Pane {
  property QtObject cityModel

  signal cityNameChanged();
  signal previousClicked();
  signal splashscreenClicked();
  signal destroyClicked();

  id: cityPanel
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
        text: i18n.t("game-editor.cities.name")
      }

      TerminalField {
        Layout.fillWidth: true
        text: cityModel.name
        onTextChanged: {
          cityModel.name = text;
          cityNameChanged();
        }
      }

      TerminalLabel {
        text: i18n.t("game-editor.cities.level")
      }

      LevelPicker {
        Layout.fillWidth: true
        value: cityModel.level
        onValueChanged: if (value !== cityModel.level) { cityModel.level = value }
      }

      TerminalLabel {
        text: i18n.t("game-editor.cities.position")
      }

      Row {
        Layout.fillWidth: true
        TerminalField {
          width: parent.width / 2
          text: cityModel.position.x
          onTextChanged: cityModel.position.x = parseInt(text)
        }
        TerminalField {
          width: parent.width / 2
          text: cityModel.position.y
          onTextChanged: cityModel.position.y = parseInt(text)
        }
      }

      TerminalLabel {
        text: "Size"
      }

      TerminalField {
        Layout.fillWidth: true
        text: cityModel.size
        onTextChanged: cityModel.size = parseInt(text)
      }
    }

    TerminalButton {
      text: i18n.t("game-editor.cities.splashscreen")
      onClicked: splashscreenClicked()
      width: parent.width
      height: 25
    }

    TerminalButton {
      text: i18n.t("game-editor.cities.remove")
      onClicked: destroyClicked()
      width: parent.width
      height: 25
    }
  }
}
