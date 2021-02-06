import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

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
    background: UiStyle.Label {
      style: parent.focus ? "dark" : "base"
    }
    onTextChanged: characterSheet.age = parseInt(text)
  }

  ComboBox {
    id: characterGender
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    enabled: root.editable
    textRole: "text"
    valueRole: "value"
    contentItem: Text {
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
      text: characterGender.currentText
      color: "white"
    }
    background: UiStyle.Label {
      style: parent.focus ? "dark" : "base"
    }
    model: [
      { value: 0, text: qsTr("Stallion") },
      { value: 1, text: qsTr("Mare") }
    ]
    onCurrentIndexChanged: {

    }
  }

  ComboBox {
    id: characterRace
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    enabled: root.editable
    textRole: "text"
    valueRole: "value"
    contentItem: Text {
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
      text: characterRace.currentText
      color: "white"
    }
    background: UiStyle.Label {
      style: parent.focus ? "dark" : "base"
    }
    model: [
      { value: 0, text: qsTr("Earth pony") },
      { value: 1, text: qsTr("Unicorn") },
      { value: 1, text: qsTr("Pegasus") }
    ]
    onCurrentIndexChanged: {

    }
  }
}
