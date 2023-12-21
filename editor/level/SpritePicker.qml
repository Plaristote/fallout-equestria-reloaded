import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../assets/ui" as UiStyle
import "../../ui"
import "../"
import "./"

ResourcePicker {
  property var animationPreviews: animationLibrary.getAnimationList(pickedOption)
  id: root
  availableItems: animationLibrary.getGroups()
  previewComponent: root.pickedOption ? spritePreview : null

  function startPicking(category) {
    open();
    currentIndex = availableItems.indexOf(category);
  }

  Component {
    id: spritePreview
    Flickable {
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
  }
}
