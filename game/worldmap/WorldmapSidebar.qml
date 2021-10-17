import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../assets/ui" as UiStyle
import "../../ui"
import "../hud" as LevelHud

Pane {
  id: sidebar
  property QtObject controller // WorldMap
  background: UiStyle.Pane {}

  ColumnLayout {
    anchors.fill: parent

    ClockWidget {
      id: clockViewContainer
      Layout.fillWidth: true
    }

    Pane {
      background: UiStyle.Pane {}
      id: cityListPane
      Layout.topMargin: 20
      Layout.bottomMargin: 20
      Layout.fillHeight: true
      Layout.fillWidth: true

      Column {
        Repeater {
          model: controller.discoveredCities
          delegate: Row {
            property QtObject city: controller.getCity(controller.discoveredCities[index]);

            UiStyle.TinyButton {
              id: cityButton
              onClicked: clickedOnCity(city)
            }
            Label {
              height: cityButton.height
              text: i18n.t(`locations.${city.name}`)
              color: "white"
              padding: 5
              background: UiStyle.Label {}
            }
          }
        }
      }
    }

    LevelHud.HudConsole {
      id: hudConsole
      gameController: gameManager.currentGame
      implicitHeight: 200
      Layout.fillWidth: true
    }

    RowLayout {
      Layout.alignment: Qt.AlignHCenter

      UiStyle.TinyButton {
        text: i18n.t("hud.INV")
        height: 20
        onClicked: actions.openInventory.trigger()
      }

      UiStyle.TinyButton {
        text: i18n.t("hud.CHA")
        height: 20
        onClicked: actions.openCharacterSheet.trigger()
      }

      UiStyle.TinyButton {
        text: i18n.t("hud.pipboy")
        height: 20
        onClicked: actions.openPipboy.trigger()
      }
    }

    UiStyle.TinyButton {
      Layout.alignment: Qt.AlignHCenter
      text: i18n.t("hud.Q")
      height: 20
      width: 20
      onClicked: actions.openMenu.trigger()
    }
  }
}
