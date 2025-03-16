import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import QtQuick.Dialogs as WindowDialogs

TerminalButton {
  id: colorButton
  property alias title: dialog.title
  property bool showAlphaChannel: true
  property alias value: dialog.selectedColor
  property color backupColor

  signal updated();

  contentItem: Rectangle {
    anchors.fill: parent
    color: value
  }
  onClicked: dialog.open()

  Timer {
    id: updateThrottle
    interval: 100
    repeat: false
    onTriggered: colorButton.updated()
  }

  WindowDialogs.ColorDialog {
    id: dialog
    options: colorButton.showAlphaChannel ? WindowDialogs.ColorDialog.ShowAlphaChannel : 0
    onSelectedColorChanged: updateThrottle.running = true
    onVisibleChanged: {
        if (visible)
          colorButton.backupColor = colorButton.value
    }
    onRejected: {
        colorButton.value = colorButton.backupColor
        colorButton.updated();
    }
  }
}
