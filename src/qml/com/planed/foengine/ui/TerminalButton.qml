import QtQuick 2.15
import QtQuick.Controls 2.15
import "./style" as UiStyle
import Game 1.0

UiStyle.TerminalButton {
  onHoveredChanged: {
    mouseCursor.setCurrentPointer(hovered ? MouseCursor.ActivePointer : MouseCursor.NormalPointer)
  }
}
