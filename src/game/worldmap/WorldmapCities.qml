import QtQuick 2.15

Repeater {
  property bool clickEnabled: false

  signal cityClicked(QtObject city)

  delegate: Rectangle {
    property QtObject city: modelData
    x: city.position.x - width / 2; y: city.position.y - height / 2
    width: city.size * 2 - border.width * 2
    height: city.size * 2 - border.width * 2
    radius: width * 0.5
    border.color: "yellow"
    border.width: 4
    color: Qt.rgba(255, 255, 0, 0.5);
    MouseArea {
      anchors.fill: parent
      enabled: clickEnabled
      onClicked: cityClicked(city)
    }
  }
}
