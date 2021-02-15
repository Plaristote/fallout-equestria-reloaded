import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import Game 1.0

DynamicObjectEditor {
  id: objectEditor

  signal openStorageClicked()

  fields: [
    TerminalButton {
      Layout.rowSpan: 2
      Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
      text: "Inventory"
      onClicked: openStorageClicked();
      padding: 20
      height: 40
    }
  ]
}
