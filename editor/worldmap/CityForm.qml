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
  signal destroyClicked();

  id: cityPanel
  background: UiStyle.TerminalPane {}
  clip: true

  Column {
    anchors.fill: parent
    spacing: 5

    TerminalButton {
      text: "< Previous"
      onClicked: previousClicked()
      height: 20
      width: parent.width
    }

    GridLayout {
      width: parent.width
      columns: 2

      TerminalLabel {
        text: "Name"
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
        text: "Level"
      }

      TerminalComboBox {
        Layout.fillWidth: true
        model: scriptController.getLevels()
        currentIndex: scriptController.getLevels().indexOf(cityModel.level)
        onCurrentTextChanged: cityModel.level = currentText
        clip: true
      }

      TerminalLabel {
        text: "Position"
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
      text: "Delete city"
      onClicked: destroyClicked()
      width: parent.width
      height: 25
    }
  }
}
