import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
  id: root
  property var templates: []
  property int currentIndex: 0
  property var currentTemplate: templates[currentIndex]

  signal customize(string templateName)
  signal accepted(string templateName)

  onVisibleChanged: {
    if (visible)
      root.forceActiveFocus();
  }

  Shortcut {
    enabled: root.visible && nextButton.visible
    sequence: StandardKey.MoveToNextChar
    onActivated: currentIndex++
  }

  Shortcut {
    enabled: root.visible && previousButton.visible
    sequence: StandardKey.MoveToPreviousChar
    onActivated: currentIndex--
  }

  Repeater {
    model: templates
    delegate: Image {
      source: assetPath + "characterTemplates/" + templates[index] + ".png"
      width: root.width
      height: root.height
      visible: currentIndex === index
    }
  }

  Text {
    color: "white"
    style: Text.Outline
    styleColor: "black"
    text: i18n.t("templates.title")
    anchors { top: parent.top; left: parent.left; topMargin: 10; leftMargin: 10 }
    font.pointSize: 20
  }

  MenuButton {
    id: previousButton
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    text: i18n.t("tutorial.previous")
    visible: currentIndex > 0
    onClicked: currentIndex--
  }

  MenuButton {
    id: nextButton
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    text: i18n.t("tutorial.next")
    visible: currentIndex < templates.length - 1
    onClicked: currentIndex++
  }

  RowLayout {
    width: parent.width
    anchors.bottom: parent.bottom
    MenuButton {
      action: exitAction
    }
    Item { Layout.fillWidth: true }
    MenuButton {
      text: i18n.t("templates.create")
      onClicked: root.customize("");
    }
    MenuButton {
      text: i18n.t("templates.customize")
      onClicked: root.customize(currentTemplate)
    }
    MenuButton {
      text: i18n.t("templates.pick")
      onClicked: root.accepted(currentTemplate)
    }
  }
}
