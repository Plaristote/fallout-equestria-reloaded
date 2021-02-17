import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../ui"
import QtQuick.Dialogs 1.2 as WindowDialogs

Dialog {
  property QtObject characterSheet
  id: faceEditor
  standardButtons: Dialog.Ok
  modal: true
  background: UiStyle.Pane {}
  width: parent.width
  height: parent.height

  WindowDialogs.ColorDialog {
    id: faceColorDialog
    title: "Color"
    onAccepted: {
      characterSheet.faceColor = Qt.rgba(color.r, color.g, color.b, 0.5);
    }
  }

  FaceDisplay {
    theme:       characterSheet.faceTheme
    color:       characterSheet.faceColor
    accessories: characterSheet.faceAccessories
    anchors.left: fieldsPanel.right
    width: parent.width - fieldsPanel.width
    height: parent.height
  }

  Pane {
    id: fieldsPanel
    background: UiStyle.TerminalPane {}
    anchors.left: faceEditor.left
    anchors.top: faceEditor.top
    height: parent.height
    width: 200

    GridLayout {
      columns: 2
      anchors.right: parent.right
      anchors.left: parent.left

      TerminalLabel { text: "Face" }
      ComboBox {
        Layout.fillWidth: true
        id: faceThemeSelect
        model: ["mare-basic"]
        currentIndex: model.indexOf(characterSheet.faceTheme)
        onCurrentIndexChanged: { characterSheet.faceTheme = model[currentIndex] }
      }

      TerminalLabel { text: "Color" }
      TerminalButton {
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        contentItem: Rectangle {
          anchors.fill: parent
          color: characterSheet.faceColor
        }
        onClicked: faceColorDialog.open()
      }
    }
  }
}
