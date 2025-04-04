import QtQuick 2.15
import QtQuick.Layouts 1.15

RowLayout {
  property int leftWidth:  338
  property int rightWidth: 242
  spacing: 0

  BorderImage {
    Layout.preferredWidth: leftWidth
    source: "qrc:/assets/ui/play-panel-left.png"
    border { left: 18; top: 15; right: 45; bottom: 15 }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
  }

  BorderImage {
    Layout.fillWidth: true
    source: "qrc:/assets/ui/play-panel-center.png"
    border { left: 0; top: 30; right: 0; bottom: 15 }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
  }
  
  BorderImage {
    Layout.preferredWidth: rightWidth
    source: "qrc:/assets/ui/play-panel-right.png"
    border { left: 50; top: 15; right: 20; bottom: 15 }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
  }
}
