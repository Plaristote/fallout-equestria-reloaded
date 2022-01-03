import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  property string scriptCategory: "behaviour"
  property var availableScripts: scriptController.getScripts(scriptCategory)
  property var filteredScripts: availableScripts
  property alias currentFilter: filterInput.text
  property int currentIndex: 0
  property string pickedOption: filteredScripts[currentIndex] || ""
  id: root
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel
  title: "Script Picker"
  height: 600
  width: 600

  function openWithCategory(category) {
    scriptCategory = category;
    currentFilter  = "";
    open();
    filterInput.forceActiveFocus()
  }

  function afterFilterUpdate() {
    const list = [];
    for (var i = 0 ; i < availableScripts.length ; ++i) {
      if (availableScripts[i].indexOf(currentFilter) >= 0)
        list.push(availableScripts[i]);
    }
    currentIndex = 0;
    filteredScripts = list;
  }

  onAvailableScriptsChanged: afterFilterUpdate()
  onCurrentFilterChanged:    afterFilterUpdate()

  Loader {
    sourceComponent: parent.visible ? shortcuts : null
  }

  Component {
    id: shortcuts
    Item {
      Shortcut {
        sequence: "Down"
        onActivated: currentIndex = currentIndex + 1 < filteredScripts.length ? currentIndex + 1 : 0
      }

      Shortcut {
        sequence: "Up"
        onActivated: currentIndex = currentIndex > 0 ? currentIndex - 1 : filteredScripts.length - 1
      }
    }
  }

  Pane {
    background: UiStyle.TerminalPane {}
    height: parent.height
    width: parent.width

    ColumnLayout {
      anchors.fill: parent

      GridLayout {
        Layout.fillWidth: true
        columns: 3
        TerminalLabel { text: "Script type:" }
        TerminalLabel { text: scriptCategory }
        TerminalToolButton {
          iconName: "add"
          onClicked: newScriptDialog.open()
        }
        TerminalLabel { text: "Filter:" }
        TerminalField {
          id: filterInput; Layout.fillWidth: true
          onAccepted: root.accept()
          Layout.columnSpan: 2
        }
      }

      Flickable {
        Layout.fillWidth: true
        Layout.fillHeight: true
        contentHeight: resultColumn.height
        clip: true
        ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

        ColumnLayout {
          id: resultColumn
          spacing: 5

          TerminalButton {
            Layout.fillWidth: true
            text: "<i>No script</i>"
            onClicked: {
              currentIndex = -1;
              accept();
            }
          }

          Repeater {
            model: filteredScripts
            delegate: TerminalButton {
              Layout.fillWidth:true
              backgroundColor: index === currentIndex ? "green" : "transparent"
              text: filteredScripts[index]
              onClicked: {
                currentIndex = index;
                accept();
              }
            } // END Button
          } // END Repeater
        } // END ColumnLayout
      } // END Flickable
    }
  }

  TextPromptDialog {
    id: newScriptDialog
    title: `Adding ${scriptCategory} script`
    parent: Overlay.overlay
    anchors.centerIn: parent
    onAccepted: {
      var newName = newScriptDialog.value;
      if (availableScripts.indexOf(newName) < 0) {
        newName = scriptController.setScript(scriptCategory, newName, "");
        root.scriptCategoryChanged() // should refresh availableScripts
      }
      root.currentIndex = availableScripts.indexOf(newName);
      console.log("New current index", root.currentIndex, newName)
      newScriptDialog.value = "";
      filterInput.forceActiveFocus();
    }
  }
}
