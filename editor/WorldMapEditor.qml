import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../game"
import "../ui"
import "./worldmap"
import "qrc:/assets/ui" as UiStyle

Item {
  id: root
  property QtObject gameController
  property QtObject worldMap

  function updateMapSize() {
    const mapSize  = Qt.size(parseInt(mapSizeWidthInput.text), parseInt(mapSizeHeightInput.text));
    const caseSize = Qt.size(parseInt(caseWidthInput.text), parseInt(caseHeightInput.text));

    worldMap.mapSize  = mapSize;
    worldMap.caseSize = caseSize;
  }

  WorldmapView {
    controller: worldMap
    anchors {
      top: parent.top; bottom: formControls.top
      left: sidebar.right; right: parent.right
    }

    WorldmapCities {
      model: worldMap.cities
      clickEnabled: true
      onCityClicked: {
        cityEditor.cityModel = city;
        tabRow.currentTab = "cities";
      }
    }
  }

  Pane {
    id: sidebar
    background: UiStyle.Pane {}
    width: 250
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
        labels: ["Cities", "Zones"]
        currentTab: tabs[0]
      }

      CityEditor {
        Layout.fillWidth: true
        Layout.fillHeight: true
        id: cityEditor
        cities: root.worldMap.cities
        worldMap: root.worldMap
        visible: tabRow.currentTab === "cities"
      }

/*
      Pane {
        Layout.fillHeight: true
        Layout.fillWidth: true
        id: zoneList
        background: UiStyle.TerminalPane {}
        visible: tabRow.currentTab === "zones"

        Column {
          width: parent.width
          spacing: 5

          TerminalButton {
            text: "Add zone"
          }

          Repeater {
            model: worldMap.zones
            delegate: TerminalButton {
              width: parent.width
              text: worldMap.zones[index].name
            }
          }
        }
      }
*/
    }
  }

  MenuButton {
    id: formControls
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    text: "Save"
    onClicked: {
      gameController.save();
      gameController.getDataEngine().saveToFile("./assets/game.json");
    }
  }
}
