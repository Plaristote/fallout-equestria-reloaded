import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"

Rectangle {
  id: root
  property QtObject item
  property int minutes: 0
  property int seconds: 10
  color: Qt.rgba(0, 0, 0, 0.5);
  anchors.fill: parent

  MouseArea {
    anchors.fill: parent
  }

  onItemChanged: {
    minutes = 0;
    seconds = 10;
  }

  Action {
    id: increaseTimeAction
    shortcut: Shortcut {
      sequence: "+"
      onActivated: increaseTimeAction.trigger()
    }
    onTriggered: {
      if (seconds === 50) {
        minutes = minutes + 1;
        seconds = 0;
      }
      else
        seconds = seconds + 10;
    }
  }

  Action {
    id: decreaseTimeAction
    shortcut: Shortcut {
      sequence: "-"
      onActivated: decreaseTimeAction.trigger()
    }
    onTriggered: {
      if (seconds === 0) {
        if (minutes > 0) {
          minutes = minutes - 1;
          seconds = 50;
        }
      }
      else
        seconds = seconds - 10;
    }
  }

  Pane {
    background: UiStyle.Pane {}
    anchors.centerIn: parent
    width: 550
    height: 150

    Text {
      id: title
      text: i18n.t("Countdown")
      font.family: application.titleFontName
      font.pixelSize: 16
      leftPadding: 20
      color: "white"
      anchors.left: parent.left
      anchors.top: parent.top
    }

    Pane {
      background: UiStyle.TerminalPane {}
      anchors {
        top: title.bottom
        bottom: countdownControls.top
        horizontalCenter: parent.horizontalCenter
      }

      Row {
        spacing: 5

        TerminalField {
          text: minutes.toString()
          readOnly: true
        }

        Text {
          text: ":"
        }

        TerminalField {
          text: seconds.toString()
          readOnly: true
        }

        Column {
          TerminalButton {
            text: "+"
            padding: 5
            action: increaseTimeAction
          }
          TerminalButton {
            text: "-"
            padding: 5
            action: decreaseTimeAction
          }
        }
      }
    }

    Row {
      id: countdownControls
      anchors { bottom: parent.bottom; right: parent.right }
      MenuButton {
        text: i18n.t("Ok")
        onClicked: {
          const timeout = minutes * 60 + seconds;

          item.setCountdown(timeout);
          root.visible = false;
        }
      }
      MenuButton {
        text: i18n.t("Cancel")
        onClicked: root.visible = false;
      }
    }
  }
}
