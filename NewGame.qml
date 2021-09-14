import QtQuick 2.15
import QtQuick.Controls 2.15
import Game 1.0
import "./ui"

Rectangle {
  id: root
  property QtObject gameController
  property QtObject statModel: gameController.getPlayerStatistics()
  property var templates: gameController.getCharacterTemplateList()
  property int currentIndex: 0
  property bool cmapEnabled: templates.length === 0
  anchors.fill: parent
  color: "black"

  Action {
    id: acceptedAction
    onTriggered: {
      statModel.faction = "player";
      gameManager.launchNewGame();
      application.popView();
    }
  }

  Action {
    id: exitAction
    text: i18n.t("Cancel")
    onTriggered: {
      application.popView()
      gameManager.endGame();
    }
  }

  CharacterTemplatePicker {
    id: templatePicker
    templates: root.templates
    anchors.fill: parent
    visible: !cmapEnabled
    onCustomize: {
      if (templateName.length > 0)
        statModel.fromTemplate(templateName);
      cmapEnabled = true;
    }
    onAccepted: {
      statModel.fromTemplate(templateName);
      acceptedAction.trigger();
    }
  }

  CharacterSheet {
    anchors.fill: parent
    characterSheet: statModel
    visible: cmapEnabled

    Component.onCompleted: {
      characterSheet.hitPoints = characterSheet.maxHitPoints
      characterSheet.specialPoints = 5;
    }

    onAccepted: acceptedAction.trigger()
    onCanceled: exitAction.trigger()
  }

  Connections {
    target: statModel
    function onStatisticsChanged() {
      statModel.hitPoints = statModel.maxHitPoints
    }
  }
}
