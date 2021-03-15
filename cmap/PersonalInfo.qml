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
    placeholderText: qsTr("Your name")
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
    placeholderText: qsTr("age")
    placeholderTextColor: "lightgray"
    validator: IntValidator { bottom: 15; top: 100 }
    font.family: application.titleFontName
    font.pixelSize: 16
    background: UiStyle.Label {
      style: parent.focus ? "dark" : "base"
    }
    onTextChanged: characterSheet.age = parseInt(text)
  }

  SelectBox {
    id: characterGender
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    enabled: root.editable
    model: characterSheet.getGendersLabels()
    Component.onCompleted: currentIndex = characterSheet.getGenders().indexOf(characterSheet.gender)
    onCurrentIndexChanged: {
      console.log("genders", characterSheet.getGenders(), currentIndex);
      characterSheet.gender = characterSheet.getGenders()[currentIndex];
    }
  }

  SelectBox {
    id: characterRace
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    enabled: root.editable
    model: characterSheet.getAvailableRacesLabels()
    Component.onCompleted: currentIndex = characterSheet.getAvailableRaces().indexOf(characterSheet.race)
    onCurrentIndexChanged: {
      console.log("races", characterSheet.getAvailableRaces(), currentIndex);
      characterSheet.race = characterSheet.getAvailableRaces()[currentIndex];
    }
  }
}
