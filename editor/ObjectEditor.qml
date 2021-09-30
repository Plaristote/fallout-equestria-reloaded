import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2 as BiDialog
import "qrc:/assets/ui" as UiStyle
import "../ui"
import Game 1.0

Item {
  id: root
  property var objectNames: itemLibrary.getObjectList()
  property var currentObject: ({})
  property alias currentName: objectSelect.currentName

  onCurrentNameChanged: {
    objectFormLoader.sourceComponent = null;
    if (currentName != "")
      objectFormLoader.sourceComponent = objectFormComponent;
  }

  onVisibleChanged: objectNames = itemLibrary.getObjectList()

  TextPromptDialog {
    id: newObjectDialog
    title: "New item"
    anchors.centerIn: parent
    function validate() {
      if (objectNames.indexOf(value) >= 0)
        validationError = value + " already exists.";
      else
        validationError = "";
      return validationError === "";
    }

    onAccepted: {
      objectNames.push(value);
      currentName = value;
      objectNamesChanged();
    }
  }

  RowLayout {
    anchors.fill: parent

    // ItemSelect
    EditorSelectPanel {
      id: objectSelect
      model: objectNames
      onNewClicked: newObjectDialog.open()
    }
    // END ItemSelect

    Loader {
      id: objectFormLoader
      Layout.fillHeight: true
      Layout.fillWidth: true
    }
  }

  Component {
    id: objectFormComponent
    ColumnLayout {
      ObjectForm {
        id: objectForm
        Layout.fillHeight: true
        Layout.fillWidth: true
        currentObject: itemLibrary.getObject(currentName) || ({})
      }

      MenuButton {
        Layout.alignment: Qt.AlignRight
        id: saveButton
        text: "Save"
        onClicked: objectForm.save()
      }
    }
  }
}
