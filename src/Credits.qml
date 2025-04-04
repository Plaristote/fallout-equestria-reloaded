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
      running: !parent.flicking && !parent.dragging
      repeat: true
      interval: 75
      onTriggered: parent.contentY = Math.min(parent.contentY + 1, parent.contentHeight - parent.height)
    }

    GridLayout {
      id: creditColumn
      anchors.horizontalCenter: parent.horizontalCenter
      columns: 2
      columnSpacing: 5
      rowSpacing: 5

      Item {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Layout.preferredHeight: root.height / 2
      }

      Repeater {
        model: credits.categories
        delegate: categoryComponent
      }
    }
  }

  Component {
    id: categoryComponent
    Repeater {
      id: categoryRepeater
      property string category: credits.categories[index]

      model: credits.categoryCount(category) + 1
      delegate: Repeater {
        id: row
        property QtObject person: credits.person(category, index - 1)
        property bool hasArtwork: person && credits.categoryHasArtwork(category)

        model: hasArtwork ? 2 : 1
        delegate: Loader {
          property string category: categoryRepeater.category
          property QtObject person: row.person

          Layout.columnSpan: row.hasArtwork ? 1 : 2
          sourceComponent: {
            if (person)
              return index === 0 ? identityComponent : artworkComponent;
            return titleComponent;
          }
        }
      }
    }
  }

  Component {
    id: titleComponent
    Text {
      text:                category
      font.family:         application.titleFont.name
      font.pointSize:      application.titleFont.bigSize
      font.capitalization: Font.Capitalize
      color:               "orange"
      Layout.alignment:    Qt.AlignCenter
    }
  }

  Component {
    id: identityComponent
    RowLayout {
      AnimatedImage {
        source: person.avatar
        Layout.alignment: Qt.AlignTop
        Layout.preferredWidth: 50
        Layout.preferredHeight: 50
      }

      ColumnLayout {
        Layout.columnSpan: person.assets.length ? 1 : 2

        Label {
          text: person.name
          font.family:    application.titleFont.name
          font.pointSize: application.titleFont.tinySize
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
