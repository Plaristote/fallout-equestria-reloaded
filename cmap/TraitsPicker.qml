import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property int maxTraits: characterSheet.getMaxTraits()
  property var availableTraits: [
    "bloody-mess","bruiser","finesse","gifted","heavy-handed","kamikaze","skilled","small-frame"
  ]
  property string selectedProperty

  background: UiStyle.TerminalPane {}

  signal selectProperty(string selectedName)

  Text {
    id: title
    text: qsTr("Traits") + " - " + (maxTraits - characterSheet.traits.length)
    color: "white"
    font.bold: true
    font.pointSize: 15
    width: parent.width
  }

  Flickable {
    clip: true
    contentHeight: row.height
    anchors { top: title.bottom; left: parent.left; bottom: parent.bottom; right: parent.right }

    RowLayout {
      id: row
      width: parent.width

      Repeater {
        model: 2
        delegate: Column {
          property int offsetIndex: index == 0 ? 0 : availableTraits.length / 2
          Layout.alignment: Qt.AlignLeft

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
                    characterSheet.traits.push(traitName);
                  else if (isPicked)
                    characterSheet.traits.splice(characterSheet.traits.indexOf(traitName), 1);
                }
              }

              Text {
                text: traitName
                color: textColor
                MouseArea { anchors.fill: parent; onClicked: selectProperty(traitName); }
              }
            }
          }
        }
      }
    }
  }
}
