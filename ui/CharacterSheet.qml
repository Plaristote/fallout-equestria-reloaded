import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../cmap" as CMAP
import "../ui"

Pane {
  id: root
  property QtObject characterSheet
  property var mode: "create"
  readonly property bool gameEditorMode: mode === "gameEditor"
  readonly property bool createMode: mode === "create" || mode === "gameEditor"
  readonly property bool editMode: mode === "edit"
  readonly property bool viewMode: mode === "view"
  property string selectedProperty: ""

  background: UiStyle.Pane {}

  signal accepted()
  signal canceled()

  CMAP.PersonalInfo {
    id: characterNameRow
    characterSheet: root.characterSheet
    editable: createMode
    anchors {
      margins: 10
      top: parent.top
      left: parent.left
    }
  }

  Button {
    id: faceEditorButton
    background: UiStyle.Label { style: parent.down ? "dark" : "base" }
    FaceDisplay {
      anchors.fill: parent
      anchors.margins: 5
      theme: characterSheet.faceTheme
      color: characterSheet.faceColor
      accessories: characterSheet.faceAccessories
    }
    onClicked: if (createMode) { faceEditor.open() }
    anchors { top: characterNameRow.top; left: characterNameRow.right; leftMargin: 10 }
    height: characterNameRow.height
    width: 50
  }

  Timer {
    running: true
    repeat: true
    interval: 100
    onTriggered: {
      const list = scriptController.getFactions();
      const index = list.indexOf(characterSheet.faction);

      factionPicker.model = list;
      factionPicker.currentIndex = index;
    }
  }

  SelectBox {
    id: factionPicker
    visible: gameEditorMode
    anchors { top: characterNameRow.top; left: faceEditorButton.right; leftMargin: 10 }
    width: 200
    height: characterNameRow.height
    onCurrentIndexChanged: {
      if (characterSheet.faction !== model[currentIndex])
        root.characterSheet.faction = model[currentIndex];
    }
  }

  FaceEditor {
    id: faceEditor
    characterSheet: root.characterSheet
  }

  RowLayout {
    id: statisticsRow
    spacing: 10

    anchors {
      margins: 10
      top: characterNameRow.bottom
    }

    ColumnLayout {
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignTop

      CMAP.Special {
        editable: createMode
        characterSheet: root.characterSheet
        onSelectProperty: root.selectedProperty = selectedName
      }

      CMAP.SpecialCounter {
        visible: createMode
        Layout.topMargin: 15
      }

      CMAP.Experience {
        editable: gameEditorMode
        characterSheet: root.characterSheet
        Layout.fillWidth: true
        Layout.topMargin: 15
      }
    }

    ColumnLayout {
      Layout.alignment: Qt.AlignTop

      CMAP.Status {
        characterSheet: root.characterSheet
        selectedProperty: root.selectedProperty
        onSelectProperty: root.selectedProperty = selectedName
        Layout.fillWidth: true
      }

      CMAP.Statistics {
        characterSheet: root.characterSheet
        selectedProperty: root.selectedProperty
        onSelectProperty: root.selectedProperty = selectedName
        Layout.fillWidth: true
      }
    }
  }

  ColumnLayout {
    id: thirdColumn
    anchors { left: statisticsRow.right; top: statisticsRow.top; bottom: controls.top; right: parent.right }
    anchors.leftMargin: 10

    CMAP.Skills {
      canEdit: root.editMode || root.gameEditorMode
      characterSheet: root.characterSheet
      selectedProperty: root.selectedProperty
      onSelectProperty: root.selectedProperty = selectedName
      Layout.alignment: Qt.AlignTop
      Layout.fillWidth: true
      Layout.fillHeight: true
    }

    RowLayout {
      visible: editMode || createMode
      Label {
        text: i18n.t("cmap.available-skill-points")
        font.family: application.titleFontName
        font.pointSize: 14
        color: "white"
        padding: 10
        leftPadding: 30
        background: UiStyle.Pane {}
        Layout.fillWidth: true
      }
      Label {
        text: characterSheet.skillPoints
        font.family: application.consoleFontName
        font.pointSize: 12
        color: "white"
        padding: 10
        background: UiStyle.TerminalPane {}
      }
    }

    CMAP.DescriptionPane {
      id: descriptionPanel
      selectedProperty: root.selectedProperty
      Layout.fillWidth: true
      Layout.minimumHeight: 200
    }
  }

  Item {
    id: tabView
    visible: !createMode

    anchors {
      top: statisticsRow.bottom
      left: statisticsRow.left
      right: statisticsRow.right
      bottom: controls.top
      topMargin: 10
    }

    TabRow {
      id: tabList
      tabs: ["perks", "reputation", "kills"]
      labels: [i18n.t("cmap.perks"), i18n.t("cmap.reputation"), i18n.t("cmap.kills")]
      currentTab: "perks"
    }

    CMAP.PerksPane {
      id: perksPane
      characterSheet: root.characterSheet
      onSelectProperty: root.selectedProperty = selectedName
      anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
      visible: tabList.currentTab === "perks"
    }

    CMAP.KillsPane {
      id: killsPane
      characterSheet: root.characterSheet
      anchors { top: tabList.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
      visible: tabList.currentTab === "kills"
    }
  }

  CMAP.TraitsPicker {
    id: traitsView
    visible: createMode
    characterSheet: root.characterSheet
    selectedProperty: root.selectedProperty
    onSelectProperty: root.selectedProperty = selectedName
    anchors {
      top: statisticsRow.bottom
      left: statisticsRow.left
      right: statisticsRow.right
      bottom: controls.top
      topMargin: 10
    }
  }

  RowLayout {
    id: controls
    anchors { bottom: parent.bottom; right: parent.right }
    MenuButton {
      visible: createMode || editMode
      text: i18n.t("confirm")
      enabled: characterSheet.acceptable
      onClicked: root.accepted()
    }
    MenuButton {
      visible: !gameEditorMode
      text: viewMode ? i18n.t("close") : i18n.t("cancel")
      onClicked: root.canceled()
    }
  }
}
