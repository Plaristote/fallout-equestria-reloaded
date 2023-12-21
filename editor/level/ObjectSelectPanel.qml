import QtQuick 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../"

EditorSelectPanel {
  id: root

  signal newFromTemplateClicked()
  signal showClicked(QtObject object)

  globalControls: RowLayout {
    Layout.preferredHeight: 20

    TerminalButton {
      text: `+ ${i18n.t("game-editor.levels.new-object")}`
      onClicked: root.newClicked()
      Layout.alignment: Qt.AlignLeft
    }

    TerminalButton {
      text: "+ From template"
      onClicked: root.newFromTemplateClicked()
      Layout.alignment: Qt.AlignRight
    }
  }

  listItem: Component {
    TerminalButton {
      onClicked: root.currentName = model.objectName;
      contentItem: Row {
        spacing: 5
        UiStyle.Icon {
          height: 15
          width: 15
          objectType: model.getObjectType()
        }
        Text {
          text: model.objectName
          color: "white"
          font.family: application.consoleFontName
        }
      }
      TerminalToolButton {
        iconName: "see"
        onClicked: showClicked(model)
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
      }
    }
  }

  filter: function(item, text) {
    return text.length === 0 || item.objectName.indexOf(text) >= 0;
  }
}
