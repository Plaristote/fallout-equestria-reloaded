import QtQuick 2.15

BorderImage {
  property int borderSize: 25
  source: "qrc:/assets/ui/pane.png"
  border { left: borderSize; top: borderSize; right: borderSize; bottom: borderSize }
  horizontalTileMode: BorderImage.Repeat
  verticalTileMode: BorderImage.Repeat
}
