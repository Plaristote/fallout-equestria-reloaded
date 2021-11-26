
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
    ColumnLayout {
      property string  category: credits.categories[index]
      Layout.alignment: Qt.AlignCenter

      Text {
        text:                category
        font.family:         application.titleFont.name
        font.pointSize:      application.titleFont.bigSize
        font.capitalization: Font.Capitalize
        color:               "orange"
        Layout.alignment:    Qt.AlignCenter
      }

      Repeater {
        model: credits.categoryCount(category)
        delegate: personComponent
      }

      Item {
        Layout.fillWidth: true
        height: 40
      }
    }
  }

  Component {
    id: personComponent
    ColumnLayout {
      property QtObject person: credits.person(parent.category, index)
      Layout.alignment: Qt.AlignCenter

      Label {
        text: person.name
        font.family:    application.titleFont.name
        font.pointSize: application.titleFont.pointSize
        color:          "white"
        Layout.alignment: Qt.AlignCenter
      }

      TerminalToolButton {
        iconName: "open"
        text: "Homepage"
        visible: person.url.length > 0
        onClicked: Qt.openUrlExternally(person.url)
        Layout.alignment: Qt.AlignCenter
      }

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

      Item {
        Layout.fillWidth: true
        height: 20
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
