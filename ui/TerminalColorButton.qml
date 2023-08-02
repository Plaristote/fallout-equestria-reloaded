import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import QtQuick.Dialogs as WindowDialogs

TerminalButton {
  id: colorButton
  property alias title: dialog.title
  property bool showAlphaChannel: true
  property color value

  signal updated();

  contentItem: Rectangle {
    anchors.fill: parent
    color: value
  }
  onClicked: dialog.open()

  WindowDialogs.ColorDialog {
    id: dialog
    options: colorButton.showAlphaChannel ? WindowDialogs.ColorDialog.ShowAlphaChannel : 0
    onAccepted: {
      colorButton.value = color;
      colorButton.updated();
    }
  }
}
