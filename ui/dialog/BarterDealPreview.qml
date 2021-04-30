import QtQuick 2.15
import QtQuick.Layouts 1.12

Item {
  property QtObject controller

  GridLayout {
    columns: 2
    Text {
      text: i18n.t("Your offer")
      color: "white"
      font.family: application.titleFontName
      font.pixelSize: 20
    }
    Text {
      text: controller.playerStash.totalValue
      color: "yellow"
      font.family: application.titleFontName
      font.pixelSize: 20
    }
    Text {
      text: i18n.t("Its offer")
      color: "white"
      font.family: application.titleFontName
      font.pixelSize: 20
    }
    Text {
      text: controller.npcStash.totalValue
      color: "yellow"
      font.family: application.titleFontName
      font.pixelSize: 20
    }
  }
}
