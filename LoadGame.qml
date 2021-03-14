import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./assets/ui" as UiStyle
import "./ui"

SaveGameUi {
  id: root
  title: i18n.t("Load")

  Action {
    id: loadGame
    onTriggered: {
      application.popAllViews();
      application.gameLoading = root.savedGameList[root.selectedIndex];
    }
  }

  slots: [
    Repeater {
      model: savedGameList
      delegate: SavedGameListItem {
        name: root.savedGameList[index]
        selected: root.selectedIndex == index
        onClicked: root.selectedIndex = index
        onDoubleClicked: { root.selectedIndex = index; loadGame.trigger(); }
        Layout.fillWidth: true
      }
    }
  ]

  controls: [
    MenuButton {
      text: i18n.t("Load")
      action: loadGame
    },
    MenuButton {
      text: i18n.t("Cancel")
      onClicked: application.popView()
    }
  ]
}
