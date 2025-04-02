import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import QtCore
import "ui"
import "game"

Window {
  id: application
  width: 1024
  height: 800
  visible: true
  color: "black"
  title: i18n.t("FoE RPG")

  property bool isFullScreen: true

  Settings {
    id: settings
    property alias x: application.x
    property alias y: application.y
    property alias width: application.width
    property alias height: application.height
    property alias fullScreen: application.isFullScreen
  }

  onIsFullScreenChanged: updateDisplay()
  Component.onCompleted: updateDisplay()

  function updateDisplay() {
    if (isFullScreen)
      application.showFullScreen();
    else
      application.showNormal();
  }

  function pickPackage(model) {
    gamePackages.pickedPackage(model);
    application.close();
  }

  Shortcut {
    sequence: "F11"
    onActivated: application.isFullScreen = !application.isFullScreen
  }

  property alias consoleFontName: consoleFont.name
  property alias titleFontName: titleFont.name

  property var consoleFont: FontStyle {
    id: consoleFont
    source: i18n.consoleFont
    pointSize: i18n.consoleFontMetrics["point"]
    tinySize:  i18n.consoleFontMetrics["tiny"]
    bigSize:   i18n.consoleFontMetrics["big"]
    hugeSize:  bigSize / 3 * 4
  }

  property var titleFont: FontStyle {
    id: titleFont
    source: i18n.titleFont
    pointSize: i18n.titleFontMetrics["point"]
    tinySize:  i18n.titleFontMetrics["tiny"]
    bigSize:   i18n.titleFontMetrics["big"]
  }

  Flickable {
    anchors.centerIn: parent
    contentHeight: laGrid.height
    contentWidth: laGrid.width
    height: Math.min(laGrid.height, application.height)
    width: Math.min(laGrid.width, application.width)
    GridLayout {
      id: laGrid
      columns: 3
      Repeater {
        model: gamePackages.packages.length * 3
        delegate: Loader {
          property QtObject model: gamePackages.packages[Math.floor(index / 3)]
          Layout.rightMargin: 20
          sourceComponent: {
            switch (index % 3) {
            case 0: return packageIconComponent;
            case 1: return packageDescriptionComponent;
            case 2: return buttonComponent;
            }
          }
        }
      }
    }
  }

  Component {
    id: packageIconComponent
    Item {
      width: 100
      height: 100
      Image {
        source: "file:" + model.logoPath
        width: Math.min(sourceSize.width, parent.width)
        height: Math.min(sourceSize.height, parent.height)
        anchors.centerIn: parent
      }
    }
  }

  Component {
    id: packageDescriptionComponent
    ColumnLayout {
      Text {
        font.family: application.titleFontName
        font.pixelSize: application.titleFont.bigSize
        text: model.name
        color: "white"
      }
      Text {
        Layout.topMargin: 10
        font.family: application.consoleFontName
        font.pixelSize: application.consoleFont.pointSize
        text: model.description
        color: "white"
      }
    }
  }

  Component {
    id: buttonComponent
    TerminalToolButton {
      text: i18n.t("Play")
      onClicked: pickPackage(model)
      iconName: "play"
    }
  }

  ColumnLayout {
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    MenuButton {
      text: i18n.t("Exit")
      onClicked: application.close()
    }
  }
}
