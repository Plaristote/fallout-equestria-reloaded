import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject model
  id: objectPreview
  background: UiStyle.TerminalPane {}
  height: objectPreviewColumn.height + 50
  anchors.margins: 10

  Column {
    id: objectPreviewColumn
    spacing: 5
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: 10
    anchors.rightMargin: 10

    ItemIcon {
      id: selectedObjectPreviewPicture
      model: objectPreview.model
      anchors.horizontalCenter: parent.horizontalCenter
      height:         sourceClipRect.height
      width:          sourceClipRect.width
    }

    Text {
      font.family: application.consoleFontName
      font.pointSize: 12
      text: i18n.t(`items.${objectPreview.model.objectName}`)
      color: "white"
    }

    Rectangle {
      height: 1
      color: "green"
      anchors { left: parent.left; right: parent.right }
    }

    Text {
      font.family: application.consoleFontName
      font.pointSize: 8
      textFormat: Text.RichText
      text: model.description
      width: parent.width
      wrapMode: Text.WordWrap
      horizontalAlignment: Text.AlignJustify
      color: "white"
    }

    Rectangle {
      height: 1
      color: "green"
      anchors { left: parent.left; right: parent.right }
    }

    Text {
      font.family: application.consoleFontName
      font.pointSize: 8
      text: i18n.t("Weight") + ": " + objectPreview.model.weight
      color: "white"
    }
  }
}
