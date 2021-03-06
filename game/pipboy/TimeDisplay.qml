import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject timeManager: gameController.timeManager
  background: UiStyle.Pane {}
  padding: 10

  signal waitClicked();

  RowLayout {
    width: parent.width
    anchors.verticalCenter: parent.verticalCenter

    Rectangle {
      color: "black"
      width: dayText.width
      height: dayText.height
      Text {
        id: dayText
        padding: 5
        text: timeManager.day
        color: "white"
      }
    }

    Rectangle {
      color: "black"
      width: monthText.width
      height: monthText.height
      Text {
        id: monthText
        padding: 5
        text: ["JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"][timeManager.month - 1]
        color: "white"
      }
    }

    Rectangle {
      color: "black"
      width: yearText.width
      height: yearText.height
      Text {
        id: yearText
        padding: 5
        text: timeManager.year
        color: "white"
      }
    }

    Rectangle {
      color: "black"
      width: hourText.width
      height: hourText.height
      Text {
        id: hourText
        padding: 5
        text: timeManager.hour + 'h' + (timeManager.minute >= 10 ? timeManager.minute : '0' + timeManager.minute)
        color: "white"
      }
    }

    Button {
      background: UiStyle.Label { style: parent.down ? "dark" : "base" }
      contentItem: Text {
        color: "white"
        text: "Wait"
      }
      onClicked: waitClicked()
    }
  }
} // END TimeManager Display
