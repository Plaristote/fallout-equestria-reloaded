import QtQuick 2.15
import "../ui"

Item {
  id: root
  property QtObject characterSheet
  property string selectedProperty

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
    selectedProperty: root.selectedProperty
    onSelectProperty: root.selectProperty(selectedName)
    anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
    visible: tabList.currentTab === "perks"
  }

  ReputationPane {
    id: reputationPane
    selectedProperty: root.selectedProperty
    onSelectProperty: root.selectProperty(selectedName)
    anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
    visible: tabList.currentTab === "reputation"
  }

  KillsPane {
    id: killsPane
    characterSheet: root.characterSheet
    selectedProperty: root.selectedProperty
    anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
    visible: tabList.currentTab === "kills"
  }
}
