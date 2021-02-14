import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property int maxTraits: characterSheet.getMaxTraits()
  property var availableTraits: characterSheet.getAvailableTraits()
  property string selectedProperty

  background: UiStyle.TerminalPane {}

  signal selectProperty(string selectedName)

  Text {
    id: title
    text: qsTr("Available traits") + ": " + (maxTraits - characterSheet.traits.length)
    color: "white"
    font.bold: true
    font.pointSize: 12
    font.family: application.consoleFontName
    width: parent.width
  }

  Rectangle {
    id: titleSeparator
    color: "green"
    height: 1
    anchors { left: parent.left; top: title.bottom; right: parent.right }
    anchors.margins: 10
  }

  Flickable {
    clip: true
    contentHeight: row.height
    anchors { top: titleSeparator.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }
    anchors.topMargin: 10

    RowLayout {
      id: row
      width: parent.width

      Repeater {
        model: 2
        delegate: Column {
          property int offsetIndex: index == 0 ? 0 : availableTraits.length / 2
          Layout.alignment: Qt.AlignLeft
          spacing: 10

          Repeater {
            model: availableTraits.length / 2
            delegate: Row {
              property string traitName: availableTraits[index + offsetIndex]
              property color textColor: traitName == selectedProperty ? "green" : "white"
              property bool isPicked: characterSheet.traits.indexOf(traitName) >= 0
              spacing: 10

              UiStyle.TerminalButton {
                backgroundColor: isPicked ? "lightgreen" : "transparent"
                text: " "
                onClicked: {
                  selectProperty(traitName);
                  if (!isPicked && characterSheet.traits.length < maxTraits)
                    characterSheet.toggleTrait(traitName, true);
                  else if (isPicked)
                    characterSheet.toggleTrait(traitName, false);
                }
              }

              Text {
                text: traitName
                color: textColor
                font.family: application.consoleFontName
                font.pointSize: 8
                MouseArea { anchors.fill: parent; onClicked: selectProperty(traitName); }
              }
            }
          }
        }
      }
    }
  }
}
