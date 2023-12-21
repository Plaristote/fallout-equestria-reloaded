import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  property var availableItems: []
  property var filteredItems: filterItems(availableItems, currentFilter)
  property alias currentFilter: filterInput.text
  property int currentIndex: 0
  property bool allowEmptyChoice: false
  property string pickedOption: (filteredItems ? filteredItems[currentIndex] : null) || ""
  property alias previewComponent: preview.sourceComponent
  property var additionalFields: []
  id: root
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel
  height: 600
  width: 800
  onOpened: {
    currentFilter = "";
    filterInput.forceActiveFocus();
  }
  onFilteredItemsChanged: {
    currentIndex = 0;
  }

  function filterItems(source, filter) {
    const list = [];
    for (let i = 0 ; i < source.length ; ++i) {
      if (source[i].indexOf(filter) >= 0)
        list.push(source[i]);
    }
    return list;
  }

  function giveActiveFocus() {
    filterInput.forceActiveFocus();
  }

  Loader {
    sourceComponent: parent.visible ? shortcuts : null
  }

  Component {
    id: shortcuts
    Item {
      Shortcut {
        sequence: "Down"
        onActivated: currentIndex = currentIndex + 1 < filteredItems.length ? currentIndex + 1 : 0
      }

      Shortcut {
        sequence: "Up"
        onActivated: currentIndex = currentIndex > 0 ? currentIndex - 1 : filteredItems.length - 1
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
        columns: 2

        Repeater {
          model: additionalFields.length * 2
          delegate: Loader {
            Layout.fillWidth: (index % 2) == 0
            sourceComponent: (index % 2) == 1 ? additionalFieldLabel : additionalFields[Math.floor(index / 2)].component
            Component {
              id: additionalFieldLabel
              TerminalLabel { text: additionalFields[Math.floor(index / 2)].label }
            }
          }
        }

        TerminalLabel { text: i18n.t("game-editor.resource-picker.filter") }
        TerminalField {
          id: filterInput;
          Layout.fillWidth: true
          onAccepted: root.accept();
        }
      }

      RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Flickable {
          Layout.fillHeight: true
          Layout.fillWidth: true
          contentHeight: resultColumn.height
          clip: true
          ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

          ColumnLayout {
            id: resultColumn
            spacing: 5

            TerminalButton {
              Layout.fillWidth: true
              visible: allowEmptyChoice
              text: `<i>${i18n.t("game-editor.resource-picker.empty-choice")}</i>`
              onClicked: {
                currentIndex = -1;
                accept();
              }
            }

            Repeater {
              model: filteredItems
              delegate: TerminalButton {
                Layout.fillWidth: true
                backgroundColor: index === currentIndex ? "green" : "transparent"
                text: filteredItems[index]
                onClicked: {
                  currentIndex = index;
                  accept();
                }
              } // END Button
            } // END Repeater
          } // END ColumnLayout
        } // END Flickable

        Loader {
          id: preview
          Layout.preferredWidth: status == Loader.Ready ? (parent.width / 2) : 0
          Layout.fillHeight: true
          clip: true
          visible: sourceComponent != null
          sourceComponent: previewComponent
        }
      }
    }
  }
}
