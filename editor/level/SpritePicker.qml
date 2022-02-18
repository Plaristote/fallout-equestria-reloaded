import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../assets/ui" as UiStyle
import "../../ui"
import "../"

UiStyle.CustomDialog {
  property var availableSprites: animationLibrary.getGroups()
  property var filteredSprites: availableSprites
  property alias currentFilter: filterInput.text
  property int currentIndex: 0
  property string pickedOption: filteredSprites[currentIndex] || ""
  property var animationPreviews: animationLibrary.getAnimationList(pickedOption)
  id: root
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel
  title: "Sprite Picker"
  height: 600
  width: 800

  function startPicking(category) {
    currentFilter = "";
    currentIndex = availableSprites.indexOf(category);
    open();
    filterInput.forceActiveFocus()
  }

  function afterFilterUpdate() {
    const list = [];
    for (var i = 0 ; i < availableSprites.length ; ++i) {
      if (availableSprites[i].indexOf(currentFilter) >= 0)
        list.push(availableSprites[i]);
    }
    currentIndex = 0;
    filteredSprites = list;
  }

  onAvailableSpritesChanged: afterFilterUpdate()
  onCurrentFilterChanged:    afterFilterUpdate()

  Loader {
    sourceComponent: parent.visible ? shortcuts : null
  }

  Component {
    id: shortcuts
    Item {
      Shortcut {
        sequence: "Down"
        onActivated: currentIndex = currentIndex + 1 < filteredSprites.length ? currentIndex + 1 : 0
      }

      Shortcut {
        sequence: "Up"
        onActivated: currentIndex = currentIndex > 0 ? currentIndex - 1 : filteredSprites.length - 1
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
        TerminalLabel { text: "Filter:" }
        TerminalField {
          id: filterInput; Layout.fillWidth: true
          onAccepted: root.accept()
        }
      }

      RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Flickable {
          Layout.fillWidth: true
          Layout.fillHeight: true
          contentHeight: resultColumn.height
          clip: true
          ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

          ColumnLayout {
            id: resultColumn
            spacing: 5

            Repeater {
              model: filteredSprites
              delegate: TerminalButton {
                Layout.fillWidth:true
                backgroundColor: index === currentIndex ? "green" : "transparent"
                text: filteredSprites[index]
                onClicked: {
                  currentIndex = index;
                  accept();
                }
              } // END Button
            } // END Repeater
          } // END ColumnLayout
        } // END Flickable

        Flickable {
          Layout.preferredWidth: parent.width / 2
          Layout.fillHeight: true
          clip: true
          ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
          contentHeight: animationPreviewGrid.height
          GridLayout {
            id: animationPreviewGrid
            columns: 2
            Repeater {
              model: Math.min(animationPreviews.length, 10)
              delegate: AnimationPreview {
                spriteGroup: root.pickedOption
                animationName: animationPreviews[index]
                Layout.preferredHeight: nativeHeight
                Layout.preferredWidth:  nativeWidth
              }
            }
          }
        }
      } // END RowLayout
    }
  }
}
