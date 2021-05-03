import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../ui"
import QtQuick.Dialogs 1.2 as WindowDialogs

UiStyle.CustomDialog {
  property QtObject characterSheet
  property var availableAccessories: ["eye-patch","eye-scar","square-glasses"]
  id: faceEditor
  standardButtons: Dialog.Ok
  modal: true
  width: parent.width
  height: parent.height
  header: null

  onCharacterSheetChanged: {
    if (characterSheet.faceTheme == "" && characterSheet.faceOptions.length > 0)
      initializeFace();
  }

  Connections {
    target: characterSheet
    function onRaceChanged() {
      initializeFace();
    }
  }

  function initializeFace() {
    characterSheet.faceTheme = characterSheet.faceOptions[0];
    characterSheet.faceColor = Qt.rgba(255, 255, 255, 0.5);
  }

  WindowDialogs.ColorDialog {
    id: faceColorDialog
    title: "Color"
    onAccepted: {
      characterSheet.faceColor = Qt.rgba(color.r, color.g, color.b, 0.5);
    }
  }

  WindowDialogs.ColorDialog {
    id: eyeColorDialog
    title: "Eye color"
    onAccepted: {
      characterSheet.eyeColor = Qt.rgba(color.r, color.g, color.b, 0.5);
    }
  }

  WindowDialogs.ColorDialog {
    id: hairColorDialog
    title: "Hair color"
    onAccepted: {
      characterSheet.hairColor = Qt.rgba(color.r, color.g, color.b, 1);
    }
  }

  FaceDisplay {
    theme:       characterSheet.faceTheme
    hairStyle:   characterSheet.hairTheme
    color:       characterSheet.faceColor
    coloured:    characterSheet.withFaceColor
    eyeColor:    characterSheet.eyeColor
    hairColor:   characterSheet.hairColor
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
    width: 400

    GridLayout {
      columns: 2
      anchors.right: parent.right
      anchors.left: parent.left

      TerminalLabel { text: "Face" }
      ComboBox {
        Layout.fillWidth: true
        id: faceThemeSelect
        model: characterSheet.faceOptions
        currentIndex: model.indexOf(characterSheet.faceTheme)
        onCurrentIndexChanged: { characterSheet.faceTheme = model[currentIndex] }
      }

      TerminalLabel { text: "Color"; visible: characterSheet.withFaceColor }
      TerminalButton {
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        visible: characterSheet.withFaceColor
        contentItem: Rectangle {
          anchors.fill: parent
          color: characterSheet.faceColor
        }
        onClicked: faceColorDialog.open()
      }

      TerminalLabel { text: "Eye color" }
      TerminalButton {
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        contentItem: Rectangle {
          anchors.fill: parent
          color: characterSheet.eyeColor
        }
        onClicked: eyeColorDialog.open()
      }

      TerminalLabel { text: "Hair style" }
      ComboBox {
        Layout.fillWidth: true
        id: hairThemeSelect
        model: characterSheet.hairOptions
        currentIndex: model.indexOf(characterSheet.hairTheme)
        onCurrentIndexChanged: { characterSheet.hairTheme = model[currentIndex] }
      }

      TerminalLabel { text: "Hair color"; visible: characterSheet.withFaceColor }
      TerminalButton {
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        visible: characterSheet.withFaceColor
        contentItem: Rectangle {
          anchors.fill: parent
          color: characterSheet.hairColor
        }
        onClicked: hairColorDialog.open()
      }

      TerminalLabel { text: "Accessories"; Layout.alignment: Qt.AlignTop }
      Column {
        Layout.fillWidth: true
        Repeater {
          model: availableAccessories
          delegate: CheckBox {
            text: '<font color="white">' + availableAccessories[index] + '</font>'
            checked: characterSheet.faceAccessories.indexOf(availableAccessories[index]) >= 0
            onClicked: characterSheet.toggleFaceAccessory(availableAccessories[index])
          }
        }
      }
    }
  }
}
