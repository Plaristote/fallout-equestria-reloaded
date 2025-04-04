import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  id: root
  property int columns: 2
  property var controls
  background: UiStyle.TerminalPane {}
  clip: true

  CustomFlickable {
    anchors.fill: parent
    contentWidth: grid.width
    contentHeight: grid.height
    ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

    GridLayout {
      id: grid
      columns: root.columns * 2
      rowSpacing: 1
      columnSpacing: 5

      Repeater {
        model: controls
        delegate: keyRow
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
