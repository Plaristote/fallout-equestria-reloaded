import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

WorldmapListEditor {
  id: root

  function onMapClicked(x, y) {
    if (currentModel) {
      const position = worldMap.getCaseAt(Qt.point(x, y));

      if (currentModel.containsCase(position.x, position.y))
        currentModel.removeCase(position.x, position.y);
      else
        currentModel.addCase(position.x, position.y);
    }
  }

  onListChanged: refreshNames()

  formComponent: Component {
    ZoneForm {
      zoneModel: root.currentModel
      onNameChanged: root.refreshNames()
      onPreviousClicked: root.selectedName = ""
      onDestroyClicked: {
        worldMap.removeZone(root.currentModel);
        root.refreshNames();
      }
    }
  }

  addDialog:  Dialog {
    title: "Add zone"
    modal: true
    anchors.centerIn: parent.parent.parent.parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    GridLayout {
      columns: 2
      Label { text: "Name" }
      TextField { id: newCityNameInput; text: "" }
    }
    onAccepted: {
      worldMap.createZone(newCityNameInput.text);
      selectedName = newCityNameInput.text;
    }
  }
}
