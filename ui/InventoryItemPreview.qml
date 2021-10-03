import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject model
  property bool withValue: false
  id: objectPreview
  background: UiStyle.TerminalPane {}
  height: objectPreviewColumn.height + 50
  anchors.margins: 5

  ColumnLayout {
    id: objectPreviewColumn
    spacing: 5
    anchors.left: parent.left
    anchors.right: parent.right

    Text {
      Layout.fillWidth: true
      font.family: application.consoleFontName
      font.pointSize: 12
      text: i18n.t(`items.${objectPreview.model.objectName}`)
      color: "white"
      wrapMode: Text.Wrap
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: 1
      color: "green"
    }

    Flickable {
      Layout.fillWidth: true
      Layout.preferredHeight: 100
      contentHeight: descText.height
      clip: true

      ScrollBar.vertical:   UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

      Text {
        id: descText
        width: parent.width - 15
        font.family: application.consoleFontName
        font.pointSize: 8
        textFormat: Text.RichText
        text: model.description
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignJustify
        color: "white"
      }
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: 1
      color: "green"
    }

    Text {
      Layout.fillWidth: true
      font.family: application.consoleFontName
      font.pointSize: 8
      text: i18n.t("Weight") + ": " + objectPreview.model.weight
      color: "white"
    }

    Text {
      Layout.fillWidth: true
      font.family: application.consoleFontName
      font.pointSize: 8
      text: i18n.t("Value") + ": " + objectPreview.model.value
      visible: withValue
      color: "white"
    }
  }
}
