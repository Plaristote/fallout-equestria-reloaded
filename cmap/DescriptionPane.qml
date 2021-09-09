import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  property string selectedProperty
  id: descriptionPanel
  background: UiStyle.PaperPane {}

  Text {
    id: descriptionTitle
    text: selectedProperty ? i18n.t("cmap." + selectedProperty) : i18n.t("Cutie Mark Acquisition Program")
    font.family: application.titleFontName
    font.pointSize: 15
    font.letterSpacing: 2
    wrapMode: Text.WordWrap
    width: parent.width
  }

  Image {
    visible: selectedProperty.length > 0
    id: descriptionPicture
    source: "qrc:/assets/cmap/no-icon.png"
    anchors.top: descriptionTitle.bottom
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    fillMode: Image.PreserveAspectFit
  }

  Flickable {
    visible: selectedProperty.length > 0
    clip: true
    contentHeight: descriptionContent.height

    anchors {
      top: descriptionTitle.bottom
      left: descriptionPicture.right
      right: parent.right
      bottom: parent.bottom
    }

    Text {
      id: descriptionContent
      text: i18n.t("cmap.descriptions." + selectedProperty)
      wrapMode: Text.WordWrap
      width: parent.width
      font.family: application.consoleFontName
      font.pointSize: 12
      font.bold: true
    }
  }
}
