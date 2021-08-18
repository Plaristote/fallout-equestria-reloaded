import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import "qrc:/assets/ui" as UiStyle
import "../ui"

Item {
  property QtObject controller

  Pane {
    visible: !controller.visible
    background: UiStyle.TerminalPane {}
    anchors.left: parent.left
    anchors.right: parent.right

    TerminalButton {
      text: i18n.t("tutorial.open")
      onClicked: controller.visible = true
      anchors.left: parent.left
      anchors.right: parent.right
    }
  }

  Pane {
    visible: controller.visible
    background: UiStyle.TerminalPane {}
    anchors.fill: parent

    RowLayout {
      id: controls
      anchors.left: parent.left
      anchors.right: parent.right

      TerminalButton {
        Layout.fillWidth: true
        text: i18n.t("tutorial.previous")
        onClicked: controller.page--
        enabled: controller.page > 0
      }
      TerminalButton {
        Layout.fillWidth: true
        text: i18n.t("tutorial.next")
        onClicked: controller.page++
        enabled: controller.page < controller.maxPage
      }
      TerminalButton {
        text: "X"
        onClicked: controller.visible = false
      }
    }

    Text {
      id: title
      anchors.top: controls.bottom
      anchors.horizontalCenter: parent.horizontalCenter
      font.pixelSize: 20
      text: `${controller.title} ${controller.page + 1}/${controller.maxPage + 1}`
      color: "white"
    }

    Flickable {
      id: htmlFlickable
      anchors.top: title.bottom
      anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
      contentHeight: htmlView.height
      //contentWidth: htmlView.width
      clip: true
      Text {
        id: htmlView
        textFormat: Text.RichText
        text: controller.html
        color: "white"
        width: 300
        wrapMode: Text.Wrap
      }
    }
  }
}
