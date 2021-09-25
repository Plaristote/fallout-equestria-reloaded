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

  property QtObject gameController
  property QtObject currentObject
  property QtObject currentGroup
  property var crumbs: currentGroup.path.split('.')

  signal newObjectClicked()
  signal newGroupClicked()
  signal showClicked(QtObject object)

  function goToPath(path) {
    const group = gameController.level.findGroup(path);

    if (group)
      currentGroup = group;
    else
      console.log("/!\\ Group", path, "not found");
  }

  Component.onCompleted: {
    currentGroup = gameController.level;
  }

  Action {
    id: addObjectAction
    text: "Add object"
    shortcut: Shortcut {
      sequence: "Ctrl+A"
      onActivated: addObjectAction.trigger()
    }
    onTriggered: newObjectClicked()
  }

  Action {
    id: addGroupAction
    text: "Add group"
    shortcut: Shortcut {
      sequence: "Ctrl+N"
      onActivated: addGroupAction.trigger()
    }
    onTriggered: newGroupClicked()
  }

  Action {
    id: cutAction
    text: "Cut"
    shortcut: Shortcut { sequence: "Ctrl+X"; onActivated: cutAction.trigger() }
    onTriggered: {
      if (currentObject) {
        const object = currentObject;

        currentObject = null;
        gameController.level.copy(object);
        gameController.level.removeObject(object);
      }
      else if (currentGroup.parent) {
        const group = currentGroup;

        currentGroup = group.parent;
        gameController.level.copy(group);
        currentGroup.parent.removeObject(group);
      }
    }
  }

  Action {
    id: copyAction
    text: "Copy"
    shortcut: Shortcut { sequence: "Ctrl+C"; onActivated: copyAction.trigger() }
    onTriggered: gameController.level.copy(currentObject || currentGroup)
  }

  Action {
    id: pasteAction
    text: "Paste"
    shortcut: Shortcut { sequence: "Ctrl+V"; onActivated: pasteAction.trigger() }
    onTriggered: gameController.level.pasteIn(currentGroup)
  }

  ColumnLayout {
    anchors.fill: parent
    Flickable {
      id: breadcrumbs
      clip: true
      contentWidth: breadcrumbsRow.width
      Layout.preferredHeight: breadcrumbsRow.height
      Layout.fillWidth: true
      Row {
        id: breadcrumbsRow
        TerminalButton {
          text: 'root'
          onClicked: currentGroup = gameController.level
        }

        Repeater {
          model: crumbs
          delegate: TerminalButton {
            text: crumbs[index]
            onClicked: goToPath(crumbs.slice(0, index - 1).join('.'))
          }
        }
      }
    }

    RowLayout {
      TerminalButton { action: addObjectAction }
      TerminalButton { action: addGroupAction  }
    }

    Flickable {
      clip: true
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
  }
}
