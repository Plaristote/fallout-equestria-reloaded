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
  signal requestCharacterView(string characterSheet)
  signal requestDialogView(string dialogName)
  signal requestSpriteView(string group)

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
      currentGroup: root.currentGroup
      withFloor: gameController.level.floorCount > 1
    }

    Flickable {
      clip: true
      visible: currentObject !== null
      Layout.fillWidth: true
      Layout.fillHeight: true
      contentHeight: objectEditor.height
      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }
      ObjectEditorLoader {
        id: objectEditor
        levelEditor: root.levelEditor
        onOpenInventory: root.openInventory(currentObject)
        onRequestCharacterView: root.requestCharacterView(characterSheet)
        onRequestSpriteView: root.requestSpriteView(group)
        onRequestDialogView: root.requestDialogView(dialogName)
        width: parent.width - 15
      }
    }

    ControlZoneEditor {
      id: controlZoneEditor
      Layout.fillWidth: true
      selectedObject: root.currentObject || root.currentGroup
      displayRoofs: displayRoofCheckbox
      displayWalls: displayWallsCheckbox
      visible: currentObject !== null || currentGroup.parent !== null
    }
  }
}
