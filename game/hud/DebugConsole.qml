import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../assets/ui" as UiStyle
import "../../ui"

Pane {
  property QtObject gameController
  property var commandHistory: []

  id: debugConsole
  background: UiStyle.Pane {}
  height: 240
  enabled: visible

  onVisibleChanged: {
    if (visible)
      debugConsoleInput.forceActiveFocus()
    else
      debugConsoleInput.focus = false;
  }

  function runCommand() {
    var newCommandHistory = commandHistory;

    newCommandHistory.push(`$> ${debugConsoleInput.text}`);
    newCommandHistory.push(gameController.consoleEval(debugConsoleInput.text));
    commandHistory = newCommandHistory;
    debugConsoleInput.clear();
    if (historyColumn.height > historyScrollZone.height)
      historyScrollZone.contentY = historyColumn.height - historyScrollZone.height - 10;
  }

  Pane {
    clip: true
    background: UiStyle.TerminalPane {}
    anchors {
      top: parent.top; bottom: debugConsoleInputContainer.top
      left: parent.left; right: parent.right
    }

    Flickable {
      id: historyScrollZone
      anchors.fill:  parent
      contentHeight: historyColumn.height
      contentWidth:  historyColumn.width

      Column {
        id: historyColumn
        Repeater {
          model: commandHistory
          delegate: TerminalLabel {
            text: commandHistory[index]
            MouseArea {
              enabled: commandHistory[index].indexOf("$> ") >= 0
              hoverEnabled: enabled
              onContainsMouseChanged: {
                parent.color = containsMouse ? "white" : "green"
              }
              onClicked: {
                debugConsoleInput.text = parent.text.substr(3);
              }
            }
          }
        }
      }
    }
  }

  Pane {
    id: debugConsoleInputContainer
    background: UiStyle.TerminalPane {}
    anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
    height: 60
    TerminalField {
      id: debugConsoleInput
      anchors.fill: parent
      anchors.margins: 2
      onAccepted: debugConsole.runCommand();
    }
  }
}
