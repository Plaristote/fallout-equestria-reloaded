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

  addDialog: TextPromptDialog {
    title: "Add city"
    anchors.centerIn: parent.parent.parent.parent
    onAccepted: {
      worldMap.createCity(value);
      selectedName = value;
    }
  }
}
