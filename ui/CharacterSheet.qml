import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../cmap" as CMAP
import "../ui"

import Game 1.0 as Game;

Pane {
  id: root
  property QtObject characterSheet
  property string mode: "create"
  readonly property bool gameEditorMode: mode === "gameEditor"
  readonly property bool createMode: mode === "create" || mode === "gameEditor"
  readonly property bool editMode: mode === "edit"
  readonly property bool viewMode: mode === "view"
  property string selectedProperty: ""

  background: UiStyle.Pane {}

  signal accepted()
  signal canceled()

  BackAction {
    id: cancelAction
    onTriggered: canceled()
  }

  Connections {
    target: gamepad
    function onBackClicked() { cancelAction.trigger() }
  }

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
    clip: true
    CharacterSpritePreview {
      anchors.fill: parent
      animationName: "walking"
      animationRepeat: true
      character: Game.Character {
        statistics: root.characterSheet
      }
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
        Layout.fillHeight: true
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
      canPickProficiencies: root.createMode || root.gameEditorMode
      characterSheet: root.characterSheet
      selectedProperty: root.selectedProperty
      onSelectProperty: root.selectedProperty = selectedName
      Layout.alignment: Qt.AlignTop
      Layout.fillWidth: true
      Layout.fillHeight: true
    }

    Loader {
      Layout.fillWidth: true
      sourceComponent: {
        if (editMode || gameEditorMode)
          return availableSkillPointWidget;
        else if (createMode)
          return availableSkillProficiencyWidget;
        return null;
      }
    }

    CMAP.DescriptionPane {
      id: descriptionPanel
      selectedProperty: root.selectedProperty
      Layout.fillWidth: true
      Layout.minimumHeight: 200
    }
  }

  Loader {
    anchors {
      top: statisticsRow.bottom
      left: statisticsRow.left
      right: statisticsRow.right
      bottom: controls.top
      topMargin: 10
    }
    sourceComponent: createMode ? traitsPickerWidget : characterDataWidget
  }

  Component {
    id: availableSkillPointWidget
    CMAP.AvailableSkillPoints { characterSheet: root.characterSheet }
  }

  Component {
    id: availableSkillProficiencyWidget
    CMAP.AvailableSkillProficiencies { characterSheet: root.characterSheet }
  }

  Component {
    id: characterDataWidget
    CMAP.CharacterData {
      characterSheet: root.characterSheet
      onSelectProperty: root.selectedProperty = selectedName
    }
  }

  Component {
    id: traitsPickerWidget
    CMAP.TraitsPicker {
      characterSheet: root.characterSheet
      selectedProperty: root.selectedProperty
      onSelectProperty: root.selectedProperty = selectedName
    }
  }

  ConfirmDialog {
    id: cancelConfirmDialog
    onAccepted: cancelAction.trigger()
    anchors.centerIn: parent
  }

  RowLayout {
    id: controls
    anchors { bottom: parent.bottom; right: parent.right }
    MenuButton {
      visible: createMode || editMode
      text: i18n.t("Confirm")
      enabled: characterSheet.acceptable
      onClicked: root.accepted()
    }
    MenuButton {
      visible: !gameEditorMode
      text: viewMode ? i18n.t("Close") : i18n.t("Cancel")
      onClicked: viewMode ? cancelAction.trigger() : cancelConfirmDialog.open()
    }
  }
}
