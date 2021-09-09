import QtQuick 2.12
import "../ui"

Item {
  id: root
  property QtObject characterSheet

  signal selectProperty(string selectedName)

  TabRow {
    id: tabList
    tabs: ["perks", "reputation", "kills"]
    labels: [i18n.t("cmap.perks"), i18n.t("cmap.reputation"), i18n.t("cmap.kills")]
    currentTab: "perks"
  }

  PerksPane {
    id: perksPane
    characterSheet: root.characterSheet
    onSelectProperty: root.selectProperty(selectedName)
    anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
    visible: tabList.currentTab === "perks"
  }

  KillsPane {
    id: killsPane
    characterSheet: root.characterSheet
    anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
    visible: tabList.currentTab === "kills"
  }
}
