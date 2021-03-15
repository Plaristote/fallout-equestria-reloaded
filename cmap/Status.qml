import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property string selectedProperty
  property var injuries: [
    "poisoned", "radiated", "eye-damage", "crippled-front-left-leg", "crippled-front-right-leg", "crippled-rear-left-leg", "crippled-rear-right-leg"
  ]

  signal selectProperty(string selectedName);

  background: UiStyle.TerminalPane {}

  ColumnLayout {
    RowLayout {
      property string propertyName: "hitPoints"
      property color textColor: selectedProperty == propertyName ? "green" : "white"

      Label {
        text: i18n.t("cmap." + parent.propertyName)
        Layout.alignment: Qt.AlignLeft
        color: parent.textColor
        font.family: application.consoleFontName
        font.pointSize: 8
        MouseArea { anchors.fill: parent; onClicked: selectProperty(parent.parent.propertyName) }
      }
      Label {
        text: characterSheet.hitPoints + '/' + characterSheet.maxHitPoints
        Layout.alignment: Qt.AlignRight
        color: parent.textColor
        font.family: application.consoleFontName
        font.pointSize: 8
        MouseArea { anchors.fill: parent; onClicked: selectProperty(parent.parent.propertyName) }
      }
    }

    Repeater {
      model: injuries
      delegate: Label {
        property string propertyName: injuries[index]
        property color textColor: selectedProperty == propertyName ? "green" : "white"

        text: i18n.t("cmap." + injuries[index])
        color: false ? textColor : "gray"
        font.family: application.consoleFontName
        font.pointSize: 8
        MouseArea { anchors.fill: parent; onClicked: selectProperty(parent.propertyName) }
      }
    }
  }
}
