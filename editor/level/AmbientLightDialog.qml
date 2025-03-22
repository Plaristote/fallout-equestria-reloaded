import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import QtQuick.Dialogs as WindowDialogs

UiStyle.CustomDialog {
  id: root
  property QtObject levelController

  title: i18n.t("game-editor.levels.ambient-light")
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok
  background: UiStyle.TerminalPane {}
  GridLayout {
    width: parent.width
    columns: 2
    TerminalLabel { text: i18n.t("game-editor.levels.ambient-light") }
    TerminalComboBox {
      Layout.fillWidth: true
      onCurrentIndexChanged: {
        switch (currentIndex) {
        case 0:
          levelController.useAmbientLight = levelController.useDaylight = false;
          break ;
        case 1:
          levelController.useDaylight = false;
          levelController.useAmbientLight = true;
          break ;
        case 2:
          levelController.useAmbientLight = levelController.useDaylight = true;
          break ;
        }
      }
      Component.onCompleted: {
        const index  = levelController.useAmbientLight ? (levelController.useDaylight ? 2 : 1) : 0;
        model        = [
          i18n.t("game-editor.levels.ambient-light-types.none"),
          i18n.t("game-editor.levels.ambient-light-types.ambient"),
          i18n.t("game-editor.levels.ambient-light-types.daylight")
        ];
        currentIndex = index;
      }
    }

    TerminalLabel { text: i18n.t("game-editor.levels.ambient-color") }
    TerminalColorButton {
      Component.onCompleted: value = levelController.ambientColor;
      onValueChanged: levelController.ambientColor = value;
      Layout.fillWidth: true
    }
  }
}
