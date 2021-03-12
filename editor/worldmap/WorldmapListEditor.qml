import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

Item {
  id: root
  property QtObject worldMap
  property var list
  property var listNames: []
  property string selectedName
  property QtObject currentModel

  property var formComponent
  property var addDialog

  function refreshNames() {
    const newArray = [];

    for (var i = 0 ; i < list.length ; ++i)
      newArray.push(list[i].name);
    listNames = newArray;
  }

  onListChanged: refreshNames()

  onSelectedNameChanged: {
    currentModel = null;
    for (var i = 0 ; i < list.length ; ++i) {
      if (list[i].name === selectedName) {
        currentModel = list[i];
        break ;
      }
    }
  }

  Loader {
    anchors.fill: parent
    sourceComponent: currentModel ? formComponent : selectComponent
  }

  Component {
    id: selectComponent
    EditorSelectPanel {
      id: citySelect
      model: listNames
      onCurrentNameChanged: root.selectedName = currentName
      onNewClicked: addDialog.open()
    }
  }
}
