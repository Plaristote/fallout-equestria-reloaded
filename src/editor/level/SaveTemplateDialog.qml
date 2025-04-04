import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  id: root
  property QtObject selectedObject

  title: "New template"
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  background: UiStyle.Pane {}
  GridLayout {
    width: parent.width
    CustomLabel { text: "Name" }
    CustomTextField {
      id: templateNameInput
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }
  }
  onAccepted: {
    gameObjectTemplates.save(templateNameInput.text, selectedObject);
  }
}
