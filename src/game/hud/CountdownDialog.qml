import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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
    id: acceptAction
    text: i18n.t("Ok")
    shortcut: Shortcut {
      sequences: ["Return", "Enter"]
      enabled: root.visible
      onActivated: acceptAction.trigger()
    }
    onTriggered: {
      const timeout = minutes * 60 + seconds;

      item.setCountdown(timeout);
      root.visible = false;
    }
  }

  Action {
    id: cancelAction
    text: i18n.t("Cancel")
    onTriggered: root.visible = false
  }

  Action {
    id: increaseTimeAction
    shortcut: Shortcut {
      sequence: "+"
      enabled: root.visible
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
      enabled: root.visible
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
    height: 200

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

      RowLayout {
        spacing: 5
        anchors.centerIn: parent

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
      MenuButton { action: acceptAction }
      MenuButton { action: cancelAction }
    }
  }
}
