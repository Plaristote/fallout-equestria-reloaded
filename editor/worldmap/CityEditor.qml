import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

WorldmapListEditor {
  id: root

  formComponent: Component {
    id: cityComponent
    CityForm {
      cityModel: root.currentModel
      onCityNameChanged: root.refreshNames()
      onPreviousClicked: root.selectedName = ""
      onDestroyClicked: {
        worldMap.removeCity(root.currentModel);
        root.refreshNames();
      }
    }
  }

  addDialog: Dialog {
    title: "Add city"
    modal: true
    anchors.centerIn: parent.parent.parent.parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    GridLayout {
      columns: 2
      Label { text: "Name" }
      TextField { id: newCityNameInput; text: "" }
    }
    onAccepted: {
      worldMap.createCity(newCityNameInput.text);
      selectedName = newCityNameInput.text;
    }
  }
}
