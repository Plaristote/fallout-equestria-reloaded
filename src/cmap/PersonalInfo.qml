import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../ui"

RowLayout {
  property QtObject characterSheet
  id: root
  property bool editable: true

  spacing: 10
  height: 50

  TextField {
    id: characterName
    text: characterSheet.name
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    readOnly: !root.editable
    color: "white"
    placeholderText: i18n.t("cmap.name-placeholder")
    placeholderTextColor: "lightgray"
    font.family: application.titleFontName
    font.pixelSize: 16
    background: UiStyle.Label {
      style: parent.focus ? "dark" : "base"
    }
    onTextChanged: characterSheet.name = text
  }

  TextField {
    id: characterAge
    text: characterSheet.age
    Layout.minimumWidth: 50
    Layout.fillHeight: true
    color: "white"
    readOnly: !root.editable
    placeholderText: i18n.t("age")
    placeholderTextColor: "lightgray"
    validator: IntValidator { bottom: 15; top: 100 }
    font.family: application.titleFontName
    font.pixelSize: 16
    background: UiStyle.Label {
      style: parent.focus ? "dark" : "base"
    }
    onTextChanged: {
      var value = parseInt(text);

      value = Math.max(value, 0);
      value = Math.min(value, characterSheet.ageRanges.lifespan);
      if (value !== characterSheet.age)
        characterSheet.age = value;
    }
  }

  SelectBox {
    id: characterGender
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    enabled: root.editable
    model: characterSheet.getGendersLabels()
    currentIndex: characterSheet.getGenders().indexOf(characterSheet.gender)
    Component.onCompleted: currentIndex = characterSheet.getGenders().indexOf(characterSheet.gender)
    onCurrentIndexChanged: characterSheet.gender = characterSheet.getGenders()[currentIndex]
  }

  SelectBox {
    id: characterRace
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    enabled: root.editable
    model: characterSheet.getAvailableRacesLabels()
    currentIndex: characterSheet.getAvailableRaces().indexOf(characterSheet.race)
    Component.onCompleted: currentIndex = characterSheet.getAvailableRaces().indexOf(characterSheet.race)
    onCurrentIndexChanged: characterSheet.race = characterSheet.getAvailableRaces()[currentIndex]
  }
}
