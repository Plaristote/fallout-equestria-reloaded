import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

Pane {
  property QtObject splashscreenModel

  signal previousClicked();

  id: splashscreenPanel
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
      columns: 2
      width: parent.width

      TerminalLabel {
        text: "Image"
      }
      TerminalField {
        Layout.fillWidth: true
        text: splashscreenModel.imagePath
        onTextChanged: if (text !== splashscreenModel.imagePath) { splashscreenModel.imagePath = text }
      }
      TerminalButton {
        Layout.fillWidth: true
        Layout.columnSpan: 2
        text: "Add entry point"
        onClicked: splashscreenModel.addEntryPoint("")
      }
    }

    Repeater {
      model: splashscreenModel.entryPoints
      delegate: GridLayout {
        property string entryPoint: splashscreenModel.entryPoints[index]
        width: parent.width
        columns: 3
        Rectangle {
          Layout.preferredHeight: 2
          Layout.fillWidth: true
          Layout.columnSpan: 3
          color: "green"
        }
        TerminalLabel {
          text: "Level"
        }
        LevelPicker {
          Layout.fillWidth: true
          Layout.columnSpan: 2
          value: entryPoint
          onValueChanged: splashscreenModel.setEntryPointAt(entryPoint, value)
        }
        TerminalLabel {
          text: "Position"
        }
        TerminalField {
          id: xInput
          Layout.fillWidth: true
          text: splashscreenModel.getPositionFor(entryPoint).x
          onTextChanged: {
            console.log("xInput changed", Qt.point(parseInt(xInput.text), parseInt(yInput.text)));
            splashscreenModel.setPositionFor(entryPoint, Qt.point(parseInt(xInput.text), parseInt(yInput.text)))
          }
        }
        TerminalField {
          id: yInput
          Layout.fillWidth: true
          text: splashscreenModel.getPositionFor(entryPoint).y
          onTextChanged: splashscreenModel.setPositionFor(entryPoint, Qt.point(parseInt(xInput.text), parseInt(yInput.text)))
        }
        TerminalButton {
          Layout.fillWidth: true
          Layout.columnSpan: 3
          text: "Delete entry point"
          height: 25
          onClicked: splashscreenModel.removeEntryPoint(entryPoint)
        }
      }
    }
  }
}
