import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"

ColumnLayout {
  id: column
  property string filter: ""
  property QtObject objectGroup
  property var sortedGroups:  objectGroup ? getSortedList(objectGroup.groups, "name")        : []
  property var sortedObjects: objectGroup ? getSortedList(objectGroup.objects, "objectName") : []

  spacing: 5

  signal showClicked(QtObject object)
  signal objectClicked(QtObject object)
  signal groupClicked(QtObject object)

  function getSortedList(source, compareProperty) {
    var array = [];
    for (var i = 0 ; i < source.length ; ++i)
      array.push(source[i]);
    return array.sort((a, b) => a[compareProperty] < b[compareProperty] ? -1 : 1);
  }

  Repeater {
    id: groupRepeater
    model: sortedGroups
    delegate: RowLayout {
      property QtObject model: sortedGroups[index]
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
    model: sortedObjects
    delegate: RowLayout {
      property QtObject model: sortedObjects[index]
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
