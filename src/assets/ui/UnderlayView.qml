import QtQuick 2.15

Rectangle {
  readonly property bool activated: state === "shown"
  property alias sourceComponent: componentLoader.sourceComponent
  property alias source: componentLoader.source
  property alias item: componentLoader.item
  property bool itemCentered: true
  id: root
  color: Qt.rgba(0, 0, 0, 0.5)
  enabled: activated
  visible: enabled

  onActivatedChanged: if (activated) { visible = true; }

  Component.onCompleted: {
    opacity = activated ? 1 : 0;
  }

  function toggle() {
    state = activated ? "hidden" : "shown";
  }

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
    hoverEnabled: false
  }

  Item {
    id: componentWrapper
    x: 0; y: 0;
    width: parent.width
    height: parent.height
    state: root.state

    states: [
      State {
        name: "shown"
        PropertyChanges { y: 0 }
      },
      State {
        name: "hidden"
        PropertyChanges { y: -root.height }
      }
    ]

    transitions: [
      Transition {
        to: "shown"
        NumberAnimation {
          target: componentWrapper
          property: "y"
          from: -root.height
          to: 0
        }
      },
      Transition {
        to: "hidden"
        NumberAnimation {
          target: componentWrapper
          property: "y"
          from: 0
          to: -root.height
        }
      }
    ]

    Loader {
      id: componentLoader
      anchors.centerIn: itemCentered ? parent : null
    }
  }
}
