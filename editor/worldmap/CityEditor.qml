import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

Item {
  id: root
  property QtObject worldMap
  property var cities
  property var cityNames: []
  property string selectedCity
  property QtObject cityModel

  function refreshCitiesNames() {
    const newArray = [];

    for (var i = 0 ; i < cities.length ; ++i)
      newArray.push(cities[i].name);
    cityNames = newArray;
  }

  onCitiesChanged: refreshCitiesNames()

  onSelectedCityChanged: {
    cityModel = null;
    for (var i = 0 ; i < cities.length ; ++i) {
      if (cities[i].name === selectedCity) {
        cityModel = cities[i];
        break ;
      }
    }
  }

  Loader {
    anchors.fill: parent
    sourceComponent: cityModel ? cityComponent : citySelectComponent
  }

  Component {
    id: citySelectComponent
    EditorSelectPanel {
      id: citySelect
      model: cityNames
      onCurrentNameChanged: root.selectedCity = currentName
      onNewClicked: addCityDialog.open()
    }
  }

  Component {
    id: cityComponent
    CityForm {
      cityModel: root.cityModel
      onCityNameChanged: root.refreshCitiesNames()
      onPreviousClicked: root.selectedCity = ""
    }
  }

  Dialog {
    id: addCityDialog
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
      selectedCity = newCityNameInput.text;
    }
  }
}
