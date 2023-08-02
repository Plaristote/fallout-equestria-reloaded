import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../ui"
import QtQuick.Dialogs as WindowDialogs

UiStyle.CustomDialog {
  property QtObject characterSheet
  property var availableAccessories: ["eye-patch","eye-scar","square-glasses"]
  id: faceEditor
  standardButtons: Dialog.Ok
  modal: true
  width: parent.width
  height: parent.height
  header: null

  onCharacterSheetChanged: initializeFace()

  Connections {
    target: characterSheet
    function onRaceChanged() {
      //characterSheet.faceTheme = "";
      initializeFace();
    }
  }

  function initializeFace() {
    /*
    if (characterSheet.faceTheme == "" && characterSheet.faceOptions.length > 0) {
      characterSheet.faceTheme = characterSheet.faceOptions[0];
      characterSheet.faceColor = Qt.rgba(255, 255, 255, 0.5);
    }
    */
  }

  Loader {
    anchors.left: fieldsPanel.right
    width: parent.width - fieldsPanel.width
    height: parent.height
    sourceComponent: characterSheet.faceTheme != "" ? faceDisplay : null
  }

  Component {
    id: faceDisplay
    FaceDisplay {
      theme:       characterSheet.faceTheme
      hairStyle:   characterSheet.hairTheme
      color:       characterSheet.faceColor
      coloured:    characterSheet.withFaceColor
      eyeColor:    characterSheet.eyeColor
      hairColor:   characterSheet.hairColor
      accessories: characterSheet.faceAccessories
    }
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
      TerminalComboBox {
        property bool ignoreFirstEvent: true
        Layout.fillWidth: true
        id: faceThemeSelect
        model: characterSheet.faceOptions
        currentIndex: model.indexOf(characterSheet.faceTheme)
        onCurrentIndexChanged: {
          if (!ignoreFirstEvent)
            characterSheet.faceTheme = model[currentIndex]
          else
            ignoreFirstEvent = false;
        }
      }

      TerminalLabel { text: "Color"; visible: characterSheet.withFaceColor }
      TerminalColorButton {
        value: characterSheet.faceColor
        onUpdated: characterSheet.faceColor = value
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        visible: characterSheet.withFaceColor
      }

      TerminalLabel { text: "Eye color" }
      TerminalColorButton {
        value: characterSheet.eyeColor
        onUpdated: characterSheet.eyeColor = value
        Layout.fillWidth: true
        Layout.preferredHeight: 40
      }

      TerminalLabel { text: "Hair style" }
      TerminalComboBox {
        property bool ignoreFirstEvent: true
        Layout.fillWidth: true
        id: hairThemeSelect
        model: characterSheet.hairOptions
        currentIndex: model.indexOf(characterSheet.hairTheme)
        onCurrentIndexChanged: {
          if (!ignoreFirstEvent)
            characterSheet.hairTheme = model[currentIndex]
          else
            ignoreFirstEvent = false;
        }
      }

      TerminalLabel { text: "Hair color"; visible: characterSheet.withFaceColor }
      TerminalColorButton {
        value: characterSheet.hairColor
        onUpdated: characterSheet.hairColor = value
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        visible: characterSheet.withFaceColor
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
