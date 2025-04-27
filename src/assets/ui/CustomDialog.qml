import QtQuick 2.15
import QtQuick.Controls 2.15 as QuickControls
import "../../ui" as Ui

QuickControls.Dialog {
  id: root
  background: Pane {}
  onOpened: okButton.forceActiveFocus()
  property alias okButton: okButton

  function validate() {
    return true;
  }

  header: QuickControls.Pane {
    background: Pane {}
    height: 50
    Text {
      anchors.centerIn: parent
      font.family: application.titleFontName
      font.pixelSize: 18
      color: "white"
      text: root.title
    }
  }

  footer: QuickControls.Pane {
    background: Pane {}
    height: 100
    Row {
      anchors.centerIn: parent
      Ui.MenuButton {
        id: okButton
        visible: (standardButtons & QuickControls.Dialog.Ok) > 0
        text: i18n.t("Ok")
        onClicked: if (validate()) { root.accept(); }
        width: 150
      }
      Ui.MenuButton {
        id: cancelButton
        visible: (standardButtons & QuickControls.Dialog.Cancel) > 0
        text: i18n.t("Cancel")
        onClicked: root.reject()
        width: 150
      }
    }
  }

  enter: Transition {
    NumberAnimation {
      property: "opacity"
      from: 0
      to: 1
      duration: 200
    }
  }

  exit: Transition {
    NumberAnimation {
      property: "opacity"
      from: 1
      to: 0
      duration: 200
    }
  }
}
