import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  id: root
  property QtObject characterSheet
  property string selectedProperty
  property bool canEdit: false
  property bool canPickProficiencies: false
  property var list: characterSheet.getSkillList()
  property int fontSize: 12

  background: UiStyle.TerminalPane {}

  signal selectProperty(string selectedName)

  function skillEdition(skillName, operation) {
    const checkMethod = `${skillName}Can${operation}`;
    const modifyMethod = `${skillName}${operation}`;

    if (characterSheet[checkMethod]())
      characterSheet[modifyMethod]();
    root.selectProperty(skillName);
  }

  CustomFlickable {
    anchors.fill: parent
    anchors.margins: 5
    contentHeight: content.height
    //contentWidth: content.width
    ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
    clip: true

    GridLayout {
      id: content
      width: parent.width - 20
      columns: 4
      rowSpacing:    3
      columnSpacing: 5
      Repeater {
        model: root.list.length * content.columns
        delegate: Loader {
          property string propertyName: root.list[Math.floor(index / content.columns)]
          property int    column:       index % content.columns
          property bool   isSelected:   propertyName == root.selectedProperty
          property color  textColor: {
            if (characterSheet && characterSheet.proficiencies.indexOf(propertyName) >= 0)
              return isSelected ? "lightgreen" : "yellow";
            return isSelected ? "green" : "white";
          }

          Layout.fillWidth: column === 1
          sourceComponent: {
            if (!characterSheet) { return null; }
            switch (column) {
            case 0: return root.canPickProficiencies ? proficiencyControl : placeholder;
            case 1: return labelColumn;
            case 2: return valueColumn;
            case 3: return root.canEdit ? skillPointControl : placeholder;
            }
          }

          MouseArea { anchors.fill: parent; onClicked: root.selectProperty(propertyName) }
        } // END Cell Loader
      } // END Repeater
    } // END Table
  } // END Flickable

  Component {
    id: placeholder
    Rectangle {
      implicitWidth: 1
      color: "transparent"
    }
  }

  Component {
    id: labelColumn
    Text {
      text: i18n.t("cmap." + propertyName)
      color: textColor
      topPadding: 5
      bottomPadding: 5
      font.pointSize: application.consoleFont.normalSize
      font.family:    application.consoleFont.name
    }
  }

  Component {
    id: valueColumn
    Text {
      horizontalAlignment: Qt.AlignRight
      text: characterSheet[propertyName] + "%"
      color: textColor
      topPadding: 5
      bottomPadding: 5
      font.pointSize: application.consoleFont.normalSize
      font.family:    application.consoleFont.name
    }
  }

  Component {
    id: skillPointControl
    Row {
      UiStyle.TerminalButton {
        enabled: characterSheet.skillPoints > 0
        text: "+"
        onClicked: skillEdition(propertyName, "Increase")
      }

      UiStyle.TerminalButton {
        text: "-"
        onClicked: skillEdition(propertyName, "Decrease")
      }
    }
  }

  Component {
    id: proficiencyControl
    UiStyle.TerminalButton {
      property bool isProficiency: characterSheet.proficiencies.indexOf(propertyName) >= 0
      implicitWidth: 35
      backgroundColor: down ? "green" : (isProficiency ? "yellow" : "transparent")
      enabled: isProficiency ? true : characterSheet.proficiencies.length < characterSheet.maxProficiencies
      onClicked: {
        selectProperty(propertyName);
        characterSheet.toggleProficiency(propertyName);
      }
    }
  }
}
