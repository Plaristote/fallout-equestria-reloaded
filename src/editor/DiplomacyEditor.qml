import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Item {
  id: root
  property QtObject dataEngine
  property alias currentFaction: factionSelect.currentName

  Connections {
    target: dataEngine
    function onDiplomacyUpdated() {
      console.log("DiplomacyEditor: onDiplomacyUpdated");
      factionSelect.model = dataEngine.getFactionList();
      //updateFactionView();
    }
  }

  onCurrentFactionChanged: updateFactionView()

  function updateFactionView() {
    factionViewLoader.sourceComponent = null
    if (currentFaction.length > 0)
      factionViewLoader.sourceComponent = factionViewComponent;
  }

  TextPromptDialog {
    id: newFactionDialog
    title: i18n.t("game-editor.factions.add")
    anchors.centerIn: parent
    onAccepted: {
      dataEngine.registerFaction(newFactionDialog.value);
    }
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectPanel {
      id: factionSelect
      model: dataEngine.getFactionList()
      onNewClicked: newFactionDialog.open()
    }

    Loader {
      id: factionViewLoader
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.rightMargin: 50
      Layout.bottomMargin: saveButton.height
    }
  }

  Component {
    id: factionViewComponent
    FactionEditor {
      id: factionEditor
      dataEngine: root.dataEngine
      currentFaction: root.currentFaction
      Connections {
        target: saveButton
        function onClicked() { factionEditor.save() }
      }
    }
  }

  MenuButton {
    id: saveButton
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Save"
  }
}
