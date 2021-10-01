import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../assets/ui" as UiStyle
import "../../ui"
import Qt.labs.settings 1.1

Pane {
  property QtObject gameController
  property var commandHistory: []
  property int historyIt: commandHistory.length

  Settings {
    id: debugConsoleSettings
    property alias commandHistory: debugConsole.commandHistory
  }

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
    const history = commandHistory;

    if (debugConsoleInput.text !== history[history.length - 1]) {
      history.push(debugConsoleInput.text);
      if (history.length > 100)
        history.splice(0, 1);
      commandHistory = history;
    }
    historyIt = commandHistory.length;
    consoleDisplay.text += `$> ${debugConsoleInput.text}\n`;
    consoleDisplay.text += gameController.consoleEval(debugConsoleInput.text) + '\n';
    debugConsoleInput.clear();
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
      contentHeight: consoleDisplay.height
      contentWidth:  consoleDisplay.width
      onContentHeightChanged: contentY = contentHeight - height
      clip: true

      TextEdit {
        id: consoleDisplay
        readOnly: true
        font.family: application.consoleFontName
        font.pointSize: 10
        font.bold: true
        color: "green"
        selectByMouse: true
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

      Keys.onUpPressed: {
        if (historyIt > 0) {
          historyIt--;
          debugConsoleInput.text = commandHistory[historyIt];
        }
      }

      Keys.onDownPressed: {
        if (historyIt < commandHistory.length) {
          historyIt++;
          debugConsoleInput.text = historyIt >= commandHistory.length ? "" : commandHistory[historyIt]
        }
      }
    }
  }
}
