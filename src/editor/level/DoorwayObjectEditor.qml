import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents
import Game 1.0

DynamicObjectEditor {
  id: doorwayEditor
  withOrientation: true
  withCover: true
  readOnlyAnimation: true

  fields: [
    TerminalLabel { text: i18n.t("game-editor.levels.doors.opened") },
    TerminalCheckBox {
      checked: model.opened
      onCheckedChanged: {
        model.opened = checked
      }
    },

    TerminalLabel { text: i18n.t("game-editor.levels.doors.locked") },
    TerminalCheckBox {
      checked: model.locked
      onCheckedChanged: {
        model.locked = checked;
      }
    },

    TerminalLabel { text: i18n.t("game-editor.levels.doors.key") },
    TerminalField {
      Layout.fillWidth: true
      text: model.keyName
      onTextChanged: {
        model.keyName = text;
      }
    },

    TerminalLabel { text: i18n.t("game-editor.levels.doors.lock-difficulty") },
    TerminalField {
      Layout.fillWidth: true
      text: model.lockpickLevel
      onTextChanged: {
        model.lockpickLevel = parseInt(text);
      }
    }
  ]
}
