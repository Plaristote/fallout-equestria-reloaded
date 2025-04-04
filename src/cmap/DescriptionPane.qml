import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  property string selectedProperty
  id: descriptionPanel
  background: UiStyle.PaperPane {}

  function getName(name) {
    switch (name.split('.')[0]) {
    case "factions":
      return i18n.t(name + '.name');
    }
    return i18n.t("cmap." + name);
  }

  function getDescription(name) {
    switch (name.split('.')[0]) {
    case "factions":
      return i18n.t(name + '.description');
    }
    return i18n.t("cmap.descriptions." + name)
  }

  Text {
    id: descriptionTitle
    text: selectedProperty ? getName(selectedProperty) : i18n.t("Cutie Mark Acquisition Program")
    font.family: application.titleFontName
    font.pointSize: application.titleFont.pointSize * 0.8
    font.letterSpacing: 2
    wrapMode: Text.WordWrap
    width: parent.width
  }

  Image {
    visible: selectedProperty.length > 0
    id: descriptionPicture
    source: "file:./assets/cmap/no-icon.png"
    anchors.top: descriptionTitle.bottom
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    width: descriptionPanel.width / 4
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
      text: getDescription(selectedProperty)
      wrapMode: Text.WordWrap
      width: parent.width
      horizontalAlignment: Text.AlignJustify
      font.family: application.consoleFont.name
      font.pointSize: application.consoleFont.bigSize
    }
  }
}
