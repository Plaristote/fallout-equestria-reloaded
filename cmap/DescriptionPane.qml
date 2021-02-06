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
    text: selectedProperty || qsTr("Cutie Mark Acquisition Program")
    font.bold: true
    font.pointSize: 15
    wrapMode: Text.WordWrap
    width: parent.width
  }

  Image {
    id: descriptionPicture
  }

  Flickable {
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
      text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In risus tellus, commodo vitae pretium at, viverra elementum velit. Phasellus et finibus magna. Praesent fermentum, tortor eu vehicula hendrerit, magna sem iaculis metus, vitae placerat quam ligula vitae nibh. In accumsan sodales tortor, nec efficitur tortor bibendum in. In hac habitasse platea dictumst. Vivamus pellentesque ex sit amet mauris fringilla porta. Phasellus mollis enim nec lacus tempus, sit amet pretium nulla vestibulum. Suspendisse non tortor quis tortor auctor posuere nec a ligula. Suspendisse a lectus sollicitudin, feugiat dolor consectetur, tempus ligula. Etiam at diam at quam rutrum fringilla. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam facilisis sodales eros eget viverra."
      wrapMode: Text.WordWrap
      width: parent.width
    }
  }
}
