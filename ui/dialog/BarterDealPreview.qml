import QtQuick 2.15
import QtQuick.Layouts 1.12
import ".."

Item {
  property QtObject controller
  property QtObject currentItem

  GridLayout {
    anchors.fill: parent

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
    Loader {
      Layout.columnSpan: 2
      Layout.fillWidth: true
      Layout.fillHeight: true
      sourceComponent: currentItem ? itemPreview : null
    }
  }

  Component {
    id: itemPreview
    ColumnLayout {
      Text {
        text: i18n.t("Item preview")
        color: "white"
        font.family: application.titleFontName
        font.pixelSize: 20
      }
      InventoryItemPreview {
        model: currentItem
        withValue: true
        Layout.fillWidth: true
        Layout.fillHeight: true
      }
    }
  }
}
