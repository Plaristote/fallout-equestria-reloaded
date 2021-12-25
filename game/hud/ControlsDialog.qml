import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  id: root
  title: i18n.t("controls-dialog")
  modal: true
  standardButtons: Dialog.Ok
  property var controls: [
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

  Shortcut {
    sequence: "F1"
    enabled: root.visible
    onActivated: root.close()
  }

  Pane {
    background: UiStyle.TerminalPane {}
    anchors.fill: parent
    clip: true

    CustomFlickable {
      anchors.fill: parent
      contentWidth: grid.width
      contentHeight: grid.height

      GridLayout {
        id: grid
        columns: 4
        rowSpacing: 1
        columnSpacing: 5

        Repeater {
          model: controls
          delegate: keyRow
        }
      }
    }
  }

  Component {
    id: keyRow
    Repeater {
      property string key: controls[index].key
      property string label: controls[index].label
      model: 2
      delegate: Loader {
        property string cellKey: key
        property string cellLabel: label
        sourceComponent: index === 0 ? keyHint : keyLabel
      }
    }
  }

  Component {
    id: keyHint
    KeyHint {
      key: cellKey
      Layout.preferredWidth: 48
    }
  }

  Component {
    id: keyLabel
    TerminalLabel {
      text: cellLabel
      Layout.fillWidth: true
    }
  }
}
