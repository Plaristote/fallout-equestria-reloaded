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

    Image {
      anchors.horizontalCenter: parent.horizontalCenter
      id: selectedObjectPreviewPicture
      source:         fileProtocol + objectPreview.model.getSpriteSource()
      sourceClipRect: objectPreview.model.getClippedRect()
      height:         objectPreview.model.getClippedRect().height
      width:          objectPreview.model.getClippedRect().width
    }

    Text {
      font.family: application.consoleFontName
      font.pointSize: 12
      text: objectPreview.model.objectName
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
      text: "Lorem ipsum dolor sit amet, consetitur subis pacem para bellum alea jacta est perseverare in vine diabolicum."
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
      text: "Weight: " + objectPreview.model.weight
      color: "white"
    }
  }
}
