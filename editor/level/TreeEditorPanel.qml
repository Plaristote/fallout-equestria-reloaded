import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"

Pane {
  id: root
  background: UiStyle.TerminalPane {}
  Layout.fillHeight: true
  Layout.preferredWidth: 200

  property QtObject levelEditor
  property QtObject gameController: levelEditor.gameController
  property alias controlZone: controlZoneEditor
  property QtObject currentObject
  property QtObject currentGroup
  property string currentPath: currentObject ? currentObject.path : currentGroup.path

  signal newObjectClicked()
  signal newGroupClicked()
  signal removeObjectClicked(QtObject object)
  signal removeGroupClicked(QtObject object)
  signal showClicked(QtObject object)
  signal openInventory(QtObject object)

  function goToPath(path) {
    if (currentPath === path)
      return ;
    currentObject = null;
    if (path.length > 0) {
      const group = gameController.level.findGroup(path);

      if (group)
        currentGroup = group;
      else
        console.log("/!\\ Group", path, "not found");
    }
    else
      currentGroup = gameController.level;
  }

  ColumnLayout {
    anchors.fill: parent

    LevelToolBar {
      gameController:     root.gameController
      currentGroup:       root.currentGroup
      currentObject:      root.currentObject
      onShowClicked:      root.showClicked(object)
      onNewObjectClicked: root.newObjectClicked()
      onNewGroupClicked:  root.newGroupClicked()
      onPathChanged:      goToPath(newPath)
      onRemoveClicked:    currentObject ? root.removeObjectClicked(currentObject) : root.removeGroupClicked(currentGroup)
    }

    Flickable {
      clip: true
      visible: currentObject === null
      Layout.fillWidth: true
      Layout.fillHeight: true
      contentHeight: treeEditorGroup.height
      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
      TreeEditorGroup {
        id: treeEditorGroup
        width: parent.width - 20
        objectGroup: currentGroup
        onShowClicked:   root.showClicked(object)
        onGroupClicked:  root.currentGroup = object
        onObjectClicked: root.currentObject = object
      }
    }

    ObjectGroupForm {
      visible: currentObject === null && currentGroup.parent !== null
      Layout.fillWidth: true
    }

    Flickable {
      clip: true
      visible: currentObject !== null
      Layout.fillWidth: true
      Layout.fillHeight: true
      contentHeight: objectEditorWrapper.height
      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
      Item {
        id: objectEditorWrapper
        width: parent.width - 20
        height: objectEditor.height + controlZoneEditor.height + 10

        ObjectEditorLoader {
          id: objectEditor
          levelEditor: root.levelEditor
          onOpenInventory: root.openInventory(currentObject)
          width: parent.width
        }

        ControlZoneEditor {
          id: controlZoneEditor
          anchors.topMargin: 10
          anchors.top: objectEditor.bottom
          width: parent.width
          selectedObject: root.currentObject
          displayRoofs: displayRoofCheckbox
          displayWalls: displayWallsCheckbox
          visible: root.currentObject !== null
        }
      }
    }
  }
}
