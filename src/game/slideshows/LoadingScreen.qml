import QtQuick 2.15

Image {
  property string backgroundName
  source: `${assetPath}backgrounds/${backgroundName.length ? backgroundName : "loading-screen"}.png`
  anchors.fill: parent
  fillMode: Image.PreserveAspectCrop

  Rectangle {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 10
    width: parent.width
    height: 100
    color: Qt.rgba(0, 0, 0, 0.5)

    Text {
      id: loadingLabel
      anchors.centerIn: parent
      color: "white"
      text: i18n.t("Loading")
      font.family: application.consoleFont.name
      font.pointSize: application.consoleFont.bigSize
    }

    Text {
      y: loadingLabel.y
      anchors.left: loadingLabel.right
      color: "white"
      text: "..."
      font.family: application.consoleFont.name
      font.pointSize: application.consoleFont.hugeSize
      Timer {
        running: root.visible
        repeat: true
        interval: 300
        onTriggered: parent.text = parent.text.length >= 3 ? '.' : parent.text + '.'
      }
    }
  }
}
