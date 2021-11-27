import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import QtQuick.Dialogs 1.2 as WindowDialogs

TerminalButton {
  id: colorButton
  property alias title: dialog.title
  property alias showAlphaChannel: dialog.showAlphaChannel
  property color value

  signal updated();

  contentItem: Rectangle {
    anchors.fill: parent
    color: value
  }
  onClicked: dialog.open()

  WindowDialogs.ColorDialog {
    id: dialog
    showAlphaChannel: true
    onAccepted: {
      colorButton.value = color;
      colorButton.updated();
    }
  }
}
