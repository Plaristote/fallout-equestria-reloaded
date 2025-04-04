import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../ui"
import "./"

ResourcePicker {
  property string scriptCategory: "behaviour"
  id: root
  availableItems: scriptController.getScripts(scriptCategory)
  allowEmptyChoice: true
  additionalFields: [
    { label: i18n.t("game-editor.resource-picker.script-type"), component: scriptCategoryPicker }
  ]

  function openWithCategory(category) {
    scriptCategory = category;
    open();
  }

  Component {
    id: scriptCategoryPicker
    RowLayout {
      TerminalLabel {
        Layout.fillWidth: true
        text: i18n.t(`game-editor.script-category.${scriptCategory}`)
      }
      TerminalToolButton {
        iconName: "add"
        onClicked: newScriptDialog.open()
      }
    }
  }

  TextPromptDialog {
    id: newScriptDialog
    title: i18n.t("game-editor.resource-picker.adding-script", { category: i18n.t(`game-editor.script-category.${scriptCategory}`) })
    parent: Overlay.overlay
    anchors.centerIn: parent
    onAccepted: {
      var newName = newScriptDialog.value;
      if (availableItems.indexOf(newName) < 0) {
        newName = scriptController.setScript(scriptCategory, newName, "");
        root.scriptCategoryChanged() // should refresh availableItems
      }
      root.currentIndex = availableItems.indexOf(newName);
      console.log("New current index", root.currentIndex, newName)
      newScriptDialog.value = "";
      root.giveActiveFocus();
    }
  }
}
