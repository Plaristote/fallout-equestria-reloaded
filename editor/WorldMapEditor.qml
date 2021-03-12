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
  property QtObject selectedZone
  property QtObject selectedCity

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
      sourceComponent: selectedZone ? zoneHintComponent : null
    }

    Component {
      id: zoneHintComponent

      Column {
        Repeater {
          model: worldMapView.controller.caseCount.height
          delegate: Row {
            property int indexY: index
            Repeater {
              model: worldMapView.controller.caseCount.width
              delegate: Rectangle {
                id: caseRectangle
                property int indexX: index
                height: worldMapView.controller.caseSize.height - 2
                width:  worldMapView.controller.caseSize.width - 2
                border.width: 1
                border.color: "green"
                color: getColor()

                function getColor() {
                  if (selectedZone.containsCase(indexX, indexY))
                    return Qt.rgba(0, 0, 255, 0.5);
                  return "transparent";
                }

                Connections {
                  target: selectedZone
                  function onCasesChanged() {
                    caseRectangle.color = caseRectangle.getColor();
                  }
                }
              }
            }
          }
        }
      }
    }

    WorldmapCities {
      model: worldMap.cities
      clickEnabled: true
      onCityClicked: {
        tabRow.currentTab = "cities";
        selectedCity = city;
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
        onCurrentTabChanged: {
          selectedZone = null;
        }
      }

      Loader {
        Layout.fillHeight: true
        Layout.fillWidth: true
        sourceComponent: tabRow.currentTab === "cities" ? cityTab : zoneTab
      }

      Component {
        id: cityTab
        CityEditor {
          id: cityEditor
          list: root.worldMap.cities
          worldMap: root.worldMap

          Connections {
            target: root
            function onSelectedCityChanged() {
              cityEditor.selectedCity = root.selectedCity.name
            }
          }
        }
      }

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
      } // END ZoneEditor component
    }
  } // END Sidebar

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
