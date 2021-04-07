import QtQuick 2.15
import "../assets/ui" as UiStyle

TerminalButton {
  property string iconName

  implicitWidth: 30
  contentItem: UiStyle.Icon {
    name: iconName
  }
}
