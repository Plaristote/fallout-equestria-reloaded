import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"

ColumnLayout {
  id: column
  property string filter: ""
  property QtObject objectGroup

  spacing: 5

  signal showClicked(QtObject object)
  signal objectClicked(QtObject object)
  signal groupClicked(QtObject object)

  Repeater {
    id: groupRepeater
    model: objectGroup.groups
    delegate: RowLayout {
      property QtObject model: objectGroup.groups[index]
      property bool passFilters: filter.length === 0 || model.path.indexOf(filter) >= 0
      spacing: 5
      Layout.fillWidth: true
      Layout.preferredHeight: passFilters ? 25 : 0
      Layout.bottomMargin: passFilters ? 5 : 0


      UiStyle.Icon {
        Layout.preferredHeight: 15
        Layout.preferredWidth: 15
        objectType: "ObjectGroup"
      }

      TerminalButton {
        onClicked: groupClicked(model)
        text: model.name
        Layout.fillWidth: true
      }
    }
  }

  Repeater {
    id: objectRepeater
    model: objectGroup.objects
    delegate: RowLayout {
      property QtObject model: objectGroup.objects[index]
      property bool passFilters: filter.length === 0 || model.path.indexOf(filter) >= 0
      spacing: 5
      Layout.fillWidth: true
      Layout.preferredHeight: passFilters ? 25 : 0
      Layout.bottomMargin: passFilters ? 5 : 0

      UiStyle.Icon {
        Layout.preferredHeight: 15
        Layout.preferredWidth: 15
        objectType: model.getObjectType()
      }

      TerminalButton {
        onClicked: objectClicked(model)
        text: model.objectName
        Layout.fillWidth: true
      }

      TerminalToolButton {
        iconName: "see"
        onClicked: showClicked(model)
      }
    }
  }
}
