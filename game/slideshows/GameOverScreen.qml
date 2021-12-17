import QtQuick 2.15

Image {
  signal clicked()

  source: assetPath + "backgrounds/game-over.png"
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
      text: i18n.t("game-over-message")
      color: "white"
    }
  }
}
