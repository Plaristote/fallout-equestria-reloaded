import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import Game 1.0 as Game
import "./ui"

Rectangle {
  id: root
  color: "black"

  Game.Credits { id: credits }

  CustomFlickable {
    anchors.fill: parent
    anchors.margins: 50
    contentHeight: creditColumn.height

    Timer {
      running: true
      repeat: true
      interval: 75
      onTriggered: parent.contentY = Math.min(parent.contentY + 1, parent.contentHeight - parent.height)
    }

    ColumnLayout {
      id: creditColumn
      width: parent.width

      Repeater {
        model: credits.categories
        delegate: categoryComponent
      }
    }
  }

  Component {
    id: categoryComponent
    GridLayout {
      property string  category: credits.categories[index]
      Layout.alignment: Qt.AlignCenter
      columns:       2
      columnSpacing: 5
      rowSpacing:    5

      Text {
        text:                category
        font.family:         application.titleFont.name
        font.pointSize:      application.titleFont.bigSize
        font.capitalization: Font.Capitalize
        color:               "orange"
        Layout.alignment:    Qt.AlignCenter
        Layout.columnSpan:   2
      }

      Repeater {
        model: credits.categoryCount(category)
        delegate: personComponent
      }

      Item {
        Layout.preferredWidth:  40
        Layout.preferredHeight: 40
        Layout.columnSpan: 2
      }
    }
  }

  Component {
    id: personComponent
    Repeater {
      property QtObject person_: credits.person(parent.category, index)
      Layout.alignment: Qt.AlignCenter
      model: credits.categoryHasArtwork(parent.category) ? 2 : 1
      delegate: Loader {
        property QtObject person: person_
        sourceComponent: index === 0 ? identityComponent : artworkComponent
      }
    }
  }

  Component {
    id: identityComponent
    ColumnLayout {
      Layout.columnSpan: person.assets.length ? 1 : 2

      Label {
        text: person.name
        font.family:    application.titleFont.name
        font.pointSize: application.titleFont.pointSize + 6
        color:          "white"
        Layout.alignment: Qt.AlignLeft
      }

      TerminalToolButton {
        iconName: "open"
        text: "Homepage"
        visible: person.url.length > 0
        onClicked: Qt.openUrlExternally(person.url)
        Layout.alignment: Qt.AlignLeft
      }

      Item {
        Layout.preferredHeight: 20
        Layout.preferredWidth:  20
      }
    }
  }

  Component {
    id: artworkComponent
    RowLayout {
      Layout.alignment: Qt.AlignCenter
      Repeater {
        model: Math.min(3, person.assets.length)
        delegate: Image {
          source: person.assets[index]
          Layout.preferredWidth: Math.min(150, sourceSize.width)
          Layout.preferredHeight: width
          fillMode: Image.PreserveAspectFit
        }
      }
    }
  }

  MenuButton {
    text: i18n.t("Back")
    onClicked: application.popAllViews()
    anchors.bottom: parent.bottom
    anchors.right: parent.right
  }
}
