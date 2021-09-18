import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
  property var pipboy
  property alias nextApp:     nextAppAction
  property alias previousApp: previousAppAction
  property alias close:       closeAction

  signal closed()

  Action {
    id: nextAppAction
    shortcut: Shortcut { sequence: "PgDown"; onActivated: nextAppAction.trigger() }
    onTriggered: {
      const appNames = pipboy.appNames;
      const index = appNames.indexOf(pipboy.currentApp);
      pipboy.currentApp = pipboy.appNames[index + 1 >= pipboy.appNames.length ? 0 : index + 1];
    }
  }

  Action {
    id: previousAppAction
    shortcut: Shortcut { sequence: "PgUp"; onActivated: previousAppAction.trigger() }
    onTriggered: {
      const appNames = pipboy.appNames;
      const index = appNames.indexOf(pipboy.currentApp);
      pipboy.currentApp = appNames[index - 1 < 0 ? appNames.length - 1 : index - 1];
    }
  }

  Action {
    id: closeAction
    shortcut: Shortcut { sequence: "Esc"; onActivated: closeAction.trigger() }
    text: i18n.t("Close")
    onTriggered: {
      closed();
      application.popView();
    }
  }

  Connections {
    target: gamepad
    function onLeftTriggerClicked() { previousAppAction.trigger(); }
    function onRightTriggerClicked() { nextAppAction.trigger(); }
  }
}
