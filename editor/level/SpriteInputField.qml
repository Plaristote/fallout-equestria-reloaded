import QtQuick 2.12
import QtQuick.Layouts 1.12
import "../../ui"

RowLayout {
  id: root
  property string scriptCategory: "behaviour"
  property QtObject model

  signal requestSpriteView(string group)

  TerminalButton {
    id: spriteInput
    Layout.fillWidth: true
    text: root.model.spriteName
    onClicked: spritePicker.startPicking(root.model.spriteName)
    SpritePicker {
      id: spritePicker
      parent: Overlay.overlay
      anchors.centerIn: parent
      onAccepted: {
        root.model.spriteName = pickedOption;
        root.model.setAnimation(root.model.getAnimation());
      }
      onClosed: spriteInput.forceActiveFocus()
    }
  }
  TerminalToolButton {
    iconName: "open"
    onClicked: root.requestSpriteView(model.spriteName)
  }
}
