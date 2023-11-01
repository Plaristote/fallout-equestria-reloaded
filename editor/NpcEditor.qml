import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import Game 1.0

Item {
  property var list: scriptController.getCharacterSheets()
  property alias currentCharacter: characterSheetSelect.currentName

  onCurrentCharacterChanged: {
    characterSheetView.sourceComponent = null;
    characterSheetView.sourceComponent = characterSheetEditor;
  }

  TextPromptDialog {
    id: newCharacterDialog
    title: "New character"
    anchors.centerIn: parent
    function validate() {
      validationError = list.indexOf(value + ".json") >= 0 ? `${value} already exists.` : "";
      return validationError === "";
    }
    onAccepted: {
      list.push(value + ".json");
      listChanged();
      currentCharacter = value + ".json";
    }
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectTree {
      id: characterSheetSelect
      treeModel: list
      onNewClicked: newCharacterDialog.open()
    }

    Loader {
      Layout.fillWidth: true
      Layout.fillHeight: true
      id: characterSheetView
    }

  }

  Component {
    id: characterSheetEditor
    CharacterSheet {
      property string nameTranslationKey: "character-names." + currentCharacter.replace(/\.json$/, '')

      mode: "gameEditor"
      characterSheet: statController
      onAccepted: {
        scriptController.saveCharacterSheet(currentCharacter, statController)
        scriptController.setTranslation(nameTranslationKey, statController.name, "characters");
      }

      StatModel {
        id: statController
        onStatisticsChanged: {
          hitPoints = maxHitPoints;
        }
      }

      Component.onCompleted: {
        scriptController.loadCharacterSheet(currentCharacter, statController);
        if (i18n.t(nameTranslationKey) != nameTranslationKey) // condition will be useless once data have been migrated
          statController.name = i18n.t(nameTranslationKey);
      }
    }
  }
}
