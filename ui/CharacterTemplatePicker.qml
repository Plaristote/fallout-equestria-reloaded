import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Game 1.0
import "../assets/ui" as UiStyle

Item {
  id: root
  property var templates: []
  property int currentIndex: 0
  property var currentTemplate: templates[currentIndex]
  property var specialAttributes: ["strength", "perception", "endurance", "charisma", "intelligence", "agility", "luck"]

  signal customize(string templateName)
  signal accepted(string templateName)

  onVisibleChanged: {
    if (visible)
      root.forceActiveFocus();
  }

  Shortcut {
    enabled: root.visible && nextButton.visible
    sequence: StandardKey.MoveToNextChar
    onActivated: currentIndex < templates.length - 1 ? currentIndex++ : null
  }

  Shortcut {
    enabled: root.visible && previousButton.visible
    sequence: StandardKey.MoveToPreviousChar
    onActivated: currentIndex > 0 ? currentIndex-- : null
  }

  Shortcut {
    enabled: root.visible
    sequences: ["Enter", "Return"]
    onActivated: accepted(currentTemplate)
  }

  function getSpecialColor(value) {
    if (value >= 7)
      return "yellow";
    else if (value <= 3)
      return "red";
    return "white";
  }

  Repeater {
    model: templates
    delegate: Image {
      source: assetPath + "characterTemplates/" + templates[index] + ".png"
      width: root.width
      height: root.height
      visible: currentIndex === index

      StatModel {
        id: character
        Component.onCompleted: fromTemplate(templates[index])
      }

      ColumnLayout {
        id: characterTitle
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.left: parent.left
        anchors.leftMargin: 10
        UiStyle.TitleText { text: character.name; font.pointSize: 30; color: "yellow"; outline: "black" }
        UiStyle.TitleText { text: i18n.t("races." + character.race); font.pointSize: 15; color: "white"; outline: "black" }
      }

      RowLayout {
        anchors.top: characterTitle.top
        anchors.right: parent.right
        anchors.rightMargin: 10

        ColumnLayout {
          Layout.fillHeight: true
          UiStyle.TitleText { text: i18n.t("cmap.traits"); visible: character.traits.length > 0; color: "yellow"; outline: "black"; font.bold: true; font.pointSize: 18 }

          Repeater {
            model: character.traits
            delegate: UiStyle.TitleText {
              text: i18n.t("cmap." + character.traits[index]);
              color: "white"
              outline: "black"
              font.capitalization: Font.Capitalize
              font.pointSize: 15
            }
          }

          Text { text: i18n.t("cmap.proficiencies"); visible: character.proficiencies.length > 0; color: "yellow"; font.bold: true; font.pointSize: 18 }

          Repeater {
            model: character.proficiencies
            delegate: UiStyle.TitleText {
              text: i18n.t("cmap." + character.proficiencies[index])
              color: "white"
              outline: "black"
              font.capitalization: Font.Capitalize
              font.pointSize: 15
            }
          }

          Item { Layout.fillHeight: true }
        } // END Traits

        ColumnLayout {
          Layout.preferredWidth: 250

          UiStyle.TitleText { text: "Special"; visible: character.traits.length > 0; color: "yellow"; outline: "black"; font.bold: true;font.pointSize: 18 }

          Repeater {
            model: specialAttributes
            delegate: Rectangle {
              border.color: "white"
              color: Qt.rgba(0, 0, 0, 0.5)
              Layout.fillWidth: true
              Layout.preferredHeight: 40
              RowLayout {
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 10
                anchors.left: parent.left
                UiStyle.TitleText { text: i18n.t("cmap." + specialAttributes[index]); Layout.preferredWidth: 200; color: "white"; outline: "black"; font.pointSize: 15 }
                UiStyle.TitleText {
                  text: character[specialAttributes[index]]
                  Layout.preferredWidth: 50
                  color: getSpecialColor(character[specialAttributes[index]])
                  outline: "black"
                  font.pointSize: 15
                }
              }
            }
          }
        } // END SPECIAL
      } // END RIGHT BOX
    }
  }

  UiStyle.TitleText {
    id: viewTitle
    color: "white"
    outline: "black"
    text: i18n.t("templates.title")
    anchors { top: parent.top; left: parent.left; topMargin: 10; leftMargin: 10 }
    font.pointSize: 20
  }

  Column {
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    spacing: 5

    MenuButton {
      id: previousButton
      text: i18n.t("tutorial.previous")
      enabled: currentIndex > 0
      onClicked: currentIndex > 0 ? currentIndex-- : null
    }

    MenuButton {
      id: nextButton
      text: i18n.t("tutorial.next")
      enabled: currentIndex < templates.length - 1
      onClicked: currentIndex < templates.length - 1 ? currentIndex++ : null
    }
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
