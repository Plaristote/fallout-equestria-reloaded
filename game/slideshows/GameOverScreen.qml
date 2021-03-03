import QtQuick 2.15

Image {
  signal clicked()

  source: "qrc:/assets/backgrounds/game-over.png"; // by LimreiArt
  anchors.fill: parent
  fillMode: Image.PreserveAspectCrop
  visible: false
  enabled: visible

  MouseArea {
    anchors.fill: parent
    onClicked: parent.clicked()
  }

  Rectangle {
    anchors {
      horizontalCenter: parent.horizontalCenter
      bottom: parent.bottom
      bottomMargin: 25
    }
    color: Qt.rgba(0, 0, 0, 0.5)
    width: gameOverText.width
    height: gameOverText.height
    Text {
      id: gameOverText
      padding: 5
      text: qsTr("Ç'en est fini de toi. Ton village est perdu et mourra de faim.")
      color: "white"
    }
  }
}
