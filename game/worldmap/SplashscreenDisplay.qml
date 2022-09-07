import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import "../hud" as LevelHud
import Game 1.0 as MyGame

Loader {
  id: root
  property QtObject controller // WorldMap
  property QtObject selectedCity

  onSelectedCityChanged: {
    if (selectedCity == null)
      splashscreenSlideOut.running = true;
  }

  Connections {
    target: controller
    function onSplashscreenEntered(city) {
      if (root.selectedCity)
        splashscreenSlideOut.running = true;
      else {
        root.selectedCity = city;
        root.sourceComponent = splashscreenView;
        splashscreenSlideIn.running = true;
      }
    }
  }

  PropertyAnimation on y {
    id: splashscreenSlideIn
    from: -root.height
    to: 0
    duration: 230
  }

  PropertyAnimation on y {
    id: splashscreenSlideOut
    from: 0
    to: -root.height
    duration: 230
    onFinished: root.sourceComponent = selectedCity = null
  }

  Component {
    id: splashscreenView
    SplashscreenView {
      Component.onCompleted: location = root.selectedCity
      location: root.selectedCity
      onEntryPointClicked: function (entryPoint) {
        controller.cityEntered(entryPoint);
        root.selectedCity = null
      }
    }
  }
}
