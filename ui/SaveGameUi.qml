import QtQuick 2.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Game 1.0
import "../assets/ui" as UiStyle

Pane {
  id: root
  property string title
  property var savedGameList: gameManager.getSavedGames()
  property int selectedIndex: 0
  property alias slots: slotList.children
  property alias controls: controlsRow.children
  property string currentSave: savedGameList[selectedIndex]
  background: UiStyle.Pane {}

  Text {
    id: viewTitle
    text: title
    color: "white"
    font.family: application.titleFont.name
    font.pointSize: 25
    anchors.topMargin: 10
    anchors.leftMargin: 10
    anchors.top: parent.top
    anchors.left: parent.left
  }

  Pane {
    id: slotListPane
    background: UiStyle.TerminalPane {}
    anchors.bottom: controlsRow.top;
    anchors.left: parent.left
    anchors.top: viewTitle.bottom
    width: parent.width * 0.7

    Flickable {
      contentHeight: slotList.height
      anchors.fill: parent
      clip: true

      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

      ColumnLayout {
        id: slotList
        width: parent.width - 15
      }
    }
  }

  Pane {
    id: savePreview
    background: UiStyle.Pane {}
    anchors.bottom: controlsRow.top
    anchors.left: slotListPane.right
    anchors.top: viewTitle.bottom
    anchors.right: parent.right

    SavePreview {
      id: savePreviewData
      name: currentSave
    }

    ColumnLayout {
      anchors.fill: parent

      Loader {
        Layout.fillWidth: true
        Layout.preferredHeight: width
        sourceComponent: savePreviewData.name.length > 0 ? saveImage : null
        Component {
          id: saveImage
          Image {
            source: savePreviewData.screenshot
            fillMode: Image.PreserveAspectFit
            cache: false
          }
        }
      }

      UiStyle.TerminalPane {
        Layout.fillWidth: true
        Layout.preferredHeight: savePreviewLayout.height + 30
        GridLayout {
          id: savePreviewLayout
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.margins: 10
          columns: 2
          columnSpacing: 5
          rowSpacing: 5

          TerminalLabel { text: i18n.t("Name") }
          TerminalLabel { text: savePreviewData.characterName; Layout.fillWidth: true }

          TerminalLabel { text: i18n.t("cmap.level") }
          TerminalLabel { text: savePreviewData.characterLevel.toString() }

          TerminalLabel {
            text: savePreviewData.place.length ? i18n.t("locations." + savePreviewData.place) : i18n.t("worldmap.title")
            Layout.fillWidth: true
            Layout.columnSpan: 2
          }

          TerminalLabel {
            text: `${savePreviewData.date} ${savePreviewData.time}`
            Layout.columnSpan: 2
          }
        }
      }

      Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
      }
    }
  }

  Row {
    id: controlsRow
    anchors { bottom: parent.bottom; right: parent.right; }
  }
}
