import QtQuick 2.15

Item {
  id: root
  property int currentIndex: 0
  property var buttonRepeater

  function moveUp() {
    if (currentIndex > 0)
      currentIndex -= 1;
    else
      currentIndex = buttonRepeater.count - 1;
    moveMouseToButtonAt(currentIndex);
  }

  function moveDown() {
    if (currentIndex + 1 < buttonRepeater.count)
      currentIndex += 1
    else
      currentIndex = 0;
    moveMouseToButtonAt(currentIndex);
  }

  function select(index) {
    controller.selectOption(controller.options[index])
    if (typeof soundManager != "undefined")
      soundManager.play("ui/term-btn-click");
  }

  function moveMouseToButton(buttonItem) {
    const buttonCenter = buttonItem.mapToItem(null,
      buttonItem.width / 2,
      buttonItem.height / 2);
    mouseCursor.setRelativePosition(buttonCenter);
  }

  function moveMouseToButtonAt(index) {
    moveMouseToButton(buttonRepeater.itemAt(index));
  }

  Connections {
    target: gamepad
    enabled: root.enabled
    function onUpClicked() { buttonNavigation.moveUp() }
    function onBottomClicked() { buttonNavigation.moveDown() }
  }
}
