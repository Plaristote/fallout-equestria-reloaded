import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  id: root
  property alias tabs: tabView.tabs
  title: i18n.t("controls-dialog")
  modal: true
  standardButtons: Dialog.Ok
  property var keyboardControls: [
    {key: "F1",  label: i18n.t("controls.help")},
    {key: "I",   label: i18n.t("controls.inventory")},
    {key: "C",   label: i18n.t("controls.centerCamera")},
    {key: "S",   label: i18n.t("controls.openSkilldex")},
    {key: "O",   label: i18n.t("controls.cmap")},
    {key: "P",   label: i18n.t("controls.pipboy")},
    {key: "1",   label: i18n.t("controls.useMode1")},
    {key: "2",   label: i18n.t("controls.useMode2")},
    {key: "Esc", label: i18n.t("controls.passTurn")},
    {key: "F2",  label: i18n.t("controls.debugConsole")}
  ]
  property var gamepadControls: ({
    "L1":       i18n.t("controls.nextUseMode1"),
    "L2":       i18n.t("controls.useMode1"),
    "L3":       i18n.t("controls.interactionMode"),
    "R1":       i18n.t("controls.nextUseMode2"),
    "R2":       i18n.t("controls.useMode2"),
    "R3":       i18n.t("controls.centerCamera"),
    "A":        i18n.t("controls.click"),
    "B":        i18n.t("controls.passTurn"),
    "X":        i18n.t("controls.inventory"),
    "Y":        i18n.t("controls.openSkilldex"),
    "Top":      i18n.t("controls.cmap"),
    "Left":     i18n.t("controls.previousTarget"),
    "Right":    i18n.t("controls.nextTarget"),
    "Bottom":   i18n.t("controls.pipboy"),
    "PadLeft":  i18n.t("controls.padLeft"),
    "PadRight": i18n.t("controls.padRight")
  })

  function initializeTabRow() {
    const labels = [];

    if (gamepad.enabled)
      tabs = ["keyboard", "gamepad"];
    else
      tabs = ["keyboard"];
    for (let i = 0 ; i < tabs.length ; ++i)
      labels.push(i18n.t(tabs[i]));
    tabView.labels = labels;
    tabView.currentTab = gamepad.connected ? "gamepad" : "keyboard";
    
  }

  Shortcut {
    sequence: "F1"
    enabled: root.visible
    onActivated: root.close()
  }

  Connections {
    target: gamepad
    function onConnectedChanged() { initializeTabRow(); }
  }

  ColumnLayout {
    anchors.fill: parent

    TabRow {
      id: tabView
      visible: tabs.length > 1
      Component.onCompleted: initializeTabRow()
    }

    ShortcutTable {
      visible: tabView.currentTab === "keyboard"
      Layout.fillHeight: true
      Layout.fillWidth: true
      controls: keyboardControls
    }

    GamepadPreview {
      visible: tabView.currentTab === "gamepad"
      Layout.fillHeight: true
      Layout.fillWidth: true
      controls: gamepadControls
    }
  }
}
