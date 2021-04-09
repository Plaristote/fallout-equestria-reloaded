import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property string selectedProperty

  signal selectProperty(string selectedName);

  background: UiStyle.TerminalPane {}

  Flickable {
    id: flickable
    anchors.fill: parent
    contentHeight: column.height
    clip: true

    ScrollBar.vertical: UiStyle.TerminalScrollbar {
      orientation: Qt.Vertical
      visible: flickable.height < column.height
    }

    ColumnLayout {
      id: column

      RowLayout {
        property string propertyName: "hitPoints"
        property color textColor: selectedProperty == propertyName ? "green" : "white"

        Text {
          text: i18n.t("cmap." + parent.propertyName)
          Layout.alignment: Qt.AlignLeft
          color: parent.textColor
          font.family: application.consoleFontName
          font.pointSize: 8
          MouseArea { anchors.fill: parent; onClicked: selectProperty(parent.parent.propertyName) }
        }
        Text {
          text: characterSheet.hitPoints + '/' + characterSheet.maxHitPoints
          Layout.alignment: Qt.AlignRight
          color: parent.textColor
          font.family: application.consoleFontName
          font.pointSize: 8
          MouseArea { anchors.fill: parent; onClicked: selectProperty(parent.parent.propertyName) }
        }
      }

      Repeater {
        model: characterSheet.buffs
        delegate: Text {
          property string propertyName: "buffs." + characterSheet.buffs[index]
          property color textColor: selectedProperty == propertyName ? "green" : "white"

          text: i18n.t(propertyName)
          color: textColor
          font.family: application.consoleFontName
          font.pointSize: 8
          MouseArea { anchors.fill: parent; onClicked: selectProperty(parent.propertyName) }
        }
      }
    }
  }
}
