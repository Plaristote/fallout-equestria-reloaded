import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "./ui"

Pane {
  property bool loaded: false
  background: UiStyle.Pane {}

  Component.onCompleted: {
    console.log("Option locale load", i18n.currentLocale, i18n.getAvailableLocales());
    localeInput.currentIndex = i18n.getAvailableLocales().indexOf(i18n.currentLocale);
    loaded = true;
  }

  Pane {
    anchors { top: parent.top; left: parent.left; right: parent.right; bottom: formControls.top }
    anchors.margins: 20
    background: UiStyle.TerminalPane {}

    GridLayout {
      columns: 2

      TerminalLabel { text: i18n.t("options.locale") }
      TerminalComboBox {
        id: localeInput
        model: i18n.getAvailableLocales()
        onCurrentIndexChanged: {
          const newLocale = i18n.getAvailableLocales()[currentIndex];

          if (i18n.currentLocale !== newLocale && loaded)
            i18n.currentLocale = newLocale;
        }
      }

      TerminalLabel { text: i18n.t("options.fullscreen") }
      TerminalCheckBox {
        id: fullscreenInput
        checked: application.isFullScreen
        onCheckedChanged: application.isFullScreen = checked
      }

      TerminalLabel { text: i18n.t("options.volume") }
      Slider {
        id: volumeInput
        from: 0
        to: 100
        Component.onCompleted: { value = musicManager.defaultVolume }
        onValueChanged: { musicManager.defaultVolume = value; }
      }
    }
  }

  MenuButton {
    id: formControls
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    onClicked: application.popView()
    text: i18n.t("exit")
  }
}
