import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  property var controls
  property int widthLeft:  Math.max(labelL2.width, labelL1.width, labelPadLeft.width, labelLeft.width, labelRight.width, labelUp.width, labelDown.width)
  property int widthRight: Math.max(labelR2.width, labelL1.width, labelPadRight.width, labelY.width, labelX.width, labelA.width, labelB.width)
  background: UiStyle.TerminalPane {}
  clip: true

  CustomFlickable {
    anchors.fill: parent
    contentWidth: gamepadImage.width + widthLeft + widthRight + 10
    contentHeight: gamepadImage.height + 20
    contentX: contentWidth / 2 - width / 2
    ScrollBar.vertical:   UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
    ScrollBar.horizontal: UiStyle.TerminalScrollbar { orientation: Qt.Horizontal }

    Image {
      id: gamepadImage
      source: "qrc:/assets/ui/gamepad.png"
      anchors.left: parent.left
      anchors.leftMargin: widthLeft
    }

    TerminalLabel {
      id: labelL2
      anchors.right:     gamepadImage.left
      anchors.top:       gamepadImage.top
      anchors.topMargin: 2
      text: controls["L2"] + " «L2"
    }

    TerminalLabel {
      id: labelL1
      anchors.right: gamepadImage.left
      anchors.top: gamepadImage.top
      anchors.topMargin: 35
      text: controls["L1"] + " «L1"
    }

    TerminalLabel {
      id: labelPadLeft
      anchors.right: gamepadImage.left
      anchors.top: gamepadImage.top
      anchors.topMargin: 90
      text: controls["PadLeft"]
    }

    TerminalLabel {
      id: labelPadLeftClick
      anchors.top: labelPadLeft.bottom
      anchors.right: labelPadLeft.right
      font.pointSize: 8
      text: controls["L3"] + " «L3"
    }

    TerminalLabel {
      id: labelLeft
      anchors.right: gamepadImage.left
      anchors.top: gamepadImage.top
      anchors.topMargin: 140
      text: controls["Left"]
    }

    TerminalLabel {
      id: labelRight
      anchors.right: gamepadImage.left
      anchors.top: gamepadImage.top
      anchors.topMargin: 180
      text: controls["Right"]
    }

    TerminalLabel {
      id: labelDown
      anchors.right: gamepadImage.left
      anchors.rightMargin: -55
      anchors.top: gamepadImage.top
      anchors.topMargin: 290
      text: i18n.t("controls.pipboy") // controls["Bottom"]
    }

    TerminalLabel {
      id: labelUp
      anchors.right: gamepadImage.left
      anchors.rightMargin: -55
      anchors.top: gamepadImage.top
      anchors.topMargin: 340
      text: i18n.t("controls.cmap") // controls["Top"]
    }

    TerminalLabel {
      id: labelR2
      anchors.left: gamepadImage.right
      anchors.top:  gamepadImage.top
      anchors.topMargin: 2
      text: "R2» " + controls["R2"]
    }

    TerminalLabel {
      id: labelR1
      anchors.left: gamepadImage.right
      anchors.top: gamepadImage.top
      anchors.topMargin: 35
      text: "R1» " + controls["R1"]
    }

    TerminalLabel {
      id: labelY
      anchors.left: gamepadImage.right
      anchors.top: gamepadImage.top
      anchors.topMargin: 63
      text: controls["Y"]
    }

    TerminalLabel {
      id: labelB
      anchors.left: gamepadImage.right
      anchors.top: gamepadImage.top
      anchors.topMargin: 85
      text: controls["B"]
    }

    TerminalLabel {
      id: labelA
      anchors.left: gamepadImage.right
      anchors.top: gamepadImage.top
      anchors.topMargin: 108
      text: controls["A"]
    }

    TerminalLabel {
      id: labelX
      anchors.left: gamepadImage.right
      anchors.top: gamepadImage.top
      anchors.topMargin: 134
      text: controls["X"]
    }

    TerminalLabel {
      id: labelPadRight
      anchors.left: gamepadImage.right
      anchors.leftMargin: -69
      anchors.top: gamepadImage.top
      anchors.topMargin: 290
      text: controls["PadRight"]
    }

    TerminalLabel {
      id: labelR3
      anchors.left: labelPadRight.left
      anchors.top: labelPadRight.bottom
      font.pointSize: 8
      text: "R3» " + controls["R3"]
    }
  }
}
