import QtQuick 2.15
import QtQuick.Layouts 1.12
import ".."

Item {
  property QtObject controller
  property QtObject playerItem
  property QtObject npcItem
  property QtObject currentItem: playerItem || npcItem

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
      text: {
        return controller.playerStash.totalValue > 0
          ? controller.playerStash.evaluateValue(controller.npc, controller.player)
          : 0;
      }
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
      text: {
        return controller.npcStash.totalValue > 0
          ? controller.npcStash.evaluateValue(controller.player, controller.npc)
          : 0;
      }
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
        evaluateValue: item => {
          if (item === playerItem)
            return item.evaluateValue(controller.npc, controller.player);
          return item.evaluateValue(controller.player, controller.npc);
        }
        Layout.fillWidth: true
        Layout.fillHeight: true
      }
    }
  }
}
