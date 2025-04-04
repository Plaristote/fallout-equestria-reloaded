import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../game/worldmap"
import "../ui"
import "./worldmap"
import "qrc:/assets/ui" as UiStyle

Item {
  id: root
  property QtObject gameController
  property QtObject worldMap
  property QtObject selectedZone
  property QtObject selectedCity
  property bool splashscreenEnabled

  function updateMapSize() {
    const mapSize  = Qt.size(parseInt(mapSizeWidthInput.text), parseInt(mapSizeHeightInput.text));
    const caseSize = Qt.size(parseInt(caseWidthInput.text), parseInt(caseHeightInput.text));

    worldMap.mapSize  = mapSize;
    worldMap.caseSize = caseSize;
  }

  WorldmapView {
    id: worldMapView
    controller: worldMap
    anchors {
      top: parent.top; bottom: formControls.top
      left: sidebar.right; right: parent.right
    }

    Loader {
      sourceComponent: selectedZone ? zoneHintComponent : citiesPreviewComponent
    }
  }

  SplashscreenView {
    id: splashscreenView
    anchors.fill: worldMapView
    location: selectedCity
    visible: tabRow.currentTab === "cities" && splashscreenEnabled
  }

  Pane {
    id: sidebar
    background: UiStyle.Pane {}
    width: 370
    anchors { top: parent.top; left: parent.left; bottom: parent.bottom }

    ColumnLayout {
      anchors.fill: parent

      MapSizePane {
        Layout.fillWidth: true
        model: worldMap
      }

      TabRow {
        id: tabRow
        tabs: ["cities", "zones"]
        labels: tabs.map(tab => i18n.t(`game-editor.worldmap-tabs.${tab}`))
        currentTab: tabs[0]
        onCurrentTabChanged: {
          selectedZone = null;
        }
      }

      Loader {
        Layout.fillHeight: true
        Layout.fillWidth: true
        sourceComponent: tabRow.currentTab === "cities" ? cityTab : zoneTab
      }

    }
  }

  MenuButton {
    id: formControls
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: i18n.t("game-editor.save")
    onClicked: {
      gameController.save();
      gameController.getDataEngine().saveToFile("./assets/game.json");
    }
  }

  // CITY COMPONENTS
  Component {
    id: cityTab
    CityEditor {
      id: cityEditor
      list: root.worldMap.cities
      worldMap: root.worldMap
      onShowSplashscreenChanged: root.splashscreenEnabled = showSplashscreen
      Component.onCompleted: root.splashscreenEnabled = false
      onCurrentModelChanged: root.selectedCity = currentModel

      Connections {
        target: root
        function onSelectedCityChanged() {
          cityEditor.selectedName = root.selectedCity.name
        }
      }
    }
  }

  Component {
    id: citiesPreviewComponent
    WorldmapCities {
      model: worldMap.cities
      clickEnabled: true
      onCityClicked: {
        tabRow.currentTab = "cities";
        selectedCity = city;
      }
    }
  }

  // ZONE COMPONENTS
  Component {
    id: zoneTab
    ZoneEditor {
      id: zoneEditor
      list: root.worldMap.zones
      worldMap: root.worldMap

      onCurrentModelChanged: {
        selectedZone = currentModel;
      }

      Connections {
        target: worldMapView
        function onMapClicked() {
          zoneEditor.onMapClicked(worldMapView.mouseX, worldMapView.mouseY);
        }
      }
    }
  }

  Component {
    id: zoneHintComponent
    ZonePreview { controller: worldMap; zone: selectedZone }
  }
}
