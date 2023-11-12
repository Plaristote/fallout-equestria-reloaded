import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "./ui"
import "./game"

Pane {
  property bool loaded: false
  background: UiStyle.Pane {}

  Component.onCompleted: {
    console.log("Option locale load", i18n.currentLocale, i18n.getAvailableLocales());
    localeInput.currentIndex = i18n.getAvailableLocales().indexOf(i18n.currentLocale);
    movementInput.currentIndex = gameManager.movementModeOption;
    loaded = true;
    pauseController.paused = true;
  }

  PauseController { id: pauseController }

  BackAction {
    id: closeAction
    text: i18n.t("Close")
    onTriggered: {
      pauseController.paused = false;
      application.popView();
    }
  }

  Connections {
    target: gamepad
    function onBackClicked() { closeAction.trigger() }
  }

  Pane {
    anchors { top: parent.top; left: parent.left; right: parent.right; bottom: formControls.top }
    anchors.margins: 20
    background: UiStyle.TerminalPane {}

    GridLayout {
      columns: 2

      TerminalLabel {
        text: i18n.t("options.title.interface")
        font.pixelSize: 30
        Layout.columnSpan: 2
      }

      TerminalLabel { text: i18n.t("options.locale") }
      TerminalComboBox {
        id: localeInput
        Layout.fillWidth: true
        model: i18n.getAvailableLocales()
        onCurrentIndexChanged: {
          const newLocale = i18n.getAvailableLocales()[currentIndex];

          if (i18n.currentLocale !== newLocale && loaded)
            i18n.currentLocale = newLocale;
        }

        TerminalButton {
          anchors.left: localeInput.right
          anchors.leftMargin: 10
          text: i18n.t("Restore")
          onClicked: localeInput.currentIndex = localeInput.indexOfValue(i18n.systemLocale);
        }
      }

      TerminalLabel { text: i18n.t("options.fullscreen") }
      TerminalCheckBox {
        id: fullscreenInput
        checked: application.isFullScreen
        onCheckedChanged: application.isFullScreen = checked
      }

      TerminalLabel {
        text: i18n.t("options.title.sounds")
        font.pixelSize: 30
        Layout.columnSpan: 2
        Layout.topMargin: 20
      }

      TerminalLabel { text: i18n.t("options.musicVolume") }
      Slider {
        id: musicVolumeInput
        Layout.fillWidth: true
        from: 0
        to: 100
        Component.onCompleted: { value = musicManager.defaultVolume; }
        onValueChanged: { musicManager.defaultVolume = value; }
      }

      TerminalLabel { text: i18n.t("options.soundEffectsVolume") }
      Slider {
        id: soundEffectsVolume
        Layout.fillWidth: true
        from: 0
        to: 100
        Component.onCompleted: { value = soundManager.defaultVolume; }
        onValueChanged: { soundManager.defaultVolume = value; }
      }

      TerminalLabel {
        text: i18n.t("options.title.gameplay")
        font.pixelSize: 30
        Layout.columnSpan: 2
        Layout.topMargin: 20
      }

      TerminalLabel { text: i18n.t("options.movementMode") }
      TerminalComboBox {
        id: movementInput
        Layout.fillWidth: true
        model: [i18n.t("options.movement.mixed"), i18n.t("options.movement.alwaysRun"), i18n.t("options.movement.alwaysWalk")]
        currentIndex: gameManager.movementModeOption
        onCurrentIndexChanged: gameManager.movementModeOption = currentIndex
      }

      TerminalLabel { text: i18n.t("options.movementCursor") }
      TerminalCheckBox {
        id: movementCursorInput
        checked: mouseCursor.withMoveCursor
        onCheckedChanged: mouseCursor.withMoveCursor = checked
      }

      TerminalLabel { text: i18n.t("options.withPlayerCropCircle") }
      TerminalCheckBox {
        id: withPlayerCropCircleInput
        checked: gameManager.withPlayerCropCircle
        onCheckedChanged: gameManager.withPlayerCropCircle = checked
      }

      TerminalLabel { text: i18n.t("options.combatSpeed") }
      Slider {
        id: combatSpeedInput
        Layout.fillWidth: true
        value: gameManager.combatSpeedOption
        from: 1
        to: 10
        onValueChanged: gameManager.combatSpeedOption = value
      }
    }
  }

  MenuButton {
    id: formControls
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    action: closeAction
  }
}
