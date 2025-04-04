import QtQuick 2.15
import QtQuick.Controls 2.15
import "../assets/ui" as UiStyle

TextField {
  id: inputField
  implicitHeight: 40
  color: "white"
  background: UiStyle.Label {
    style: parent.focus ? "dark" : "base"
  }
}
