import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Item {
  id: menuPane

  signal openMenu()
  signal openInventory()

  Column {
    anchors.fill: parent
    //anchors.topMargin: 10
    spacing: 5

    UiStyle.TinyButton {
      text: i18n.t("hud.INV")
      height: 20
      width: parent.width
      onClicked: menuPane.openInventory()
    }

    UiStyle.TinyButton {
      text: i18n.t("hud.CHA")
      height: 20
      width: parent.width
      onClicked: {
        application.pushView("game/CharacterView.qml", {gameController: root.gameController})
      }
    }

    UiStyle.TinyButton {
      text: i18n.t("hud.Q")
      height: 20
      width: parent.width
      onClicked: menuPane.openMenu()
    }
  }
}
