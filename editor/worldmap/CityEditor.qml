import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

WorldmapListEditor {
  id: root
  property bool showSplashscreen: false

  formComponent: showSplashscreen ? splashscreenComponent : mapComponent

  Component {
    id: mapComponent
    CityForm {
      cityModel: root.currentModel
      onCityNameChanged: root.refreshNames()
      onPreviousClicked: root.selectedName = ""
      onSplashscreenClicked: showSplashscreen = true
      onDestroyClicked: {
        worldMap.removeCity(root.currentModel);
        root.refreshNames();
      }
    }
  }

  Component {
    id: splashscreenComponent
    SplashscreenForm {
      splashscreenModel: root.currentModel.splashscreen
      onPreviousClicked: showSplashscreen = false
    }
  }

  addDialog: TextPromptDialog {
    title: "Add city"
    parent: Overlay.overlay
    anchors.centerIn: parent
    onAccepted: {
      worldMap.createCity(value);
      selectedName = value;
    }
  }
}
