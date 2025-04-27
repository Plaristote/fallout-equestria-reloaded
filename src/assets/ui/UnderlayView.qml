import QtQuick 2.15

Rectangle {
  readonly property bool activated: state === "shown"
  id: root
  color: Qt.rgba(0, 0, 0, 0.5)
  visible: activated

  Component.onCompleted: {
    opacity = activated ? 1 : 0;
  }

  function toggle() { state = activated ? "hidden" : "shown" }

  states: [
    State {
      name: "shown"
      PropertyChanges { opacity: 1 }
    },
    State {
      name: "hidden"
      PropertyChanges { opacity: 0 }
    }
  ]

  transitions: [
    Transition {
      to: "shown"
      SequentialAnimation {
        NumberAnimation {
          target: root
          property: "opacity"
          from: 0
          to: 1
          duration: 5
        }
        ColorAnimation {
          target: root
          property: "color"
          from: "transparent"
          to: Qt.rgba(0, 0, 0, 0.5)
          duration: 250
        }
      }
    },
    Transition {
      to: "hidden"
      SequentialAnimation {
        ColorAnimation {
          target: root
          property: "color"
          from: Qt.rgba(0, 0, 0, 0.5)
          to: "transparent"
          duration: 250
        }
        NumberAnimation {
          target: root
          property: "opacity"
          from: 1
          to: 0
          duration: 5
        }
      }
    }
  ]

  MouseArea {
    anchors.fill: parent
    enabled: root.state == "shown"
    onClicked: console.log("PUTAIN")
  }
}
