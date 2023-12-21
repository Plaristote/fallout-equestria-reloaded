import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  id: root
  property alias model: repeater.model
  property alias globalControls: controlButtons.sourceComponent
  property var listItem
  property var filter
  property string currentName
  property bool readOnly: false
  background: UiStyle.TerminalPane {}
  Layout.fillHeight: true
  Layout.preferredWidth: 200

  signal newClicked()

  listItem: defaultListItem

  filter: function(item, text) {
    return text.length === 0 || item.toLowerCase().indexOf(text.toLowerCase()) >= 0;
  }

  Component {
    id: defaultGlobalControls
    TerminalButton {
      text: `+ ${i18n.t("game-editor.add")}`
      onClicked: newClicked()
      height: 20
    }
  }

  Component {
    id: defaultListItem
    TerminalButton {
      id: button
      text: model
      hoverEnabled: true
      onClicked: { root.currentName = model; }
      backgroundColor: root.currentName === model || down ? "green" : "transparent"
      UiStyle.TerminalToolTip { target: button }
    }
  }

  Flickable {
    anchors.fill: parent
    contentHeight: column.height
    clip: true
    ColumnLayout {
      id: column
      spacing: 0
      width: parent.width

      Loader {
        id: controlButtons
        sourceComponent: defaultGlobalControls
        visible: !readOnly
        Layout.fillWidth: true
        Layout.bottomMargin: 5
      }

      TerminalField {
        id: filterInput
        visible: model && model.length > 0
        Layout.fillWidth: true
        Layout.bottomMargin: 5
        placeholderText: i18n.t("game-editor.filter-placeholder")
      }

      Repeater {
        id: repeater
        delegate: RowLayout {
          spacing: 5
          Layout.fillWidth: true

          Loader {
            property var model: repeater.model[index]
            property bool passFilters: root.filter(model, filterInput.text) ? 25 : 0
            sourceComponent: listItem
            Layout.fillWidth: true
            Layout.preferredHeight: passFilters ? 25 : 0
            Layout.bottomMargin: passFilters ? 5 : 0
            opacity: passFilters ? 1 : 0
          }
        }
      }
    }
  }
}
// END ItemSelect
