import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../ui"

RowLayout {
  id: root
  property QtObject gameController
  property QtObject currentObject
  property QtObject currentGroup

  signal pathChanged(string newPath)
  signal newObjectClicked()
  signal newGroupClicked()
  signal removeClicked(QtObject object)
  signal showClicked(QtObject object)

  // Actions
  Action {
    id: addObjectAction
    text: i18n.t("game-editor.levels.add-object")
    shortcut: Shortcut { sequence: "Ctrl+A"; onActivated: addObjectAction.trigger() }
    onTriggered: newObjectClicked()
  }

  Action {
    id: addGroupAction
    text: i18n.t("game-editor.levels.add-group")
    shortcut: Shortcut { sequence: "Ctrl+N"; onActivated: addGroupAction.trigger() }
    onTriggered: newGroupClicked()
  }

  Action {
    id: removeAction
    text: i18n.t("game-editor.remove")
    shortcut: Shortcut { sequence: "Ctrl+Del"; onActivated: addGroupAction.trigger() }
    onTriggered: removeClicked(currentObject || currentGroup)
  }

  Action {
    id: cutAction
    text: i18n.t("game-editor.cut")
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
    text: i18n.t("game-editor.copy")
    shortcut: Shortcut { sequence: "Ctrl+C"; onActivated: copyAction.trigger() }
    onTriggered: {
      console.log("LevelToolBar: copy", currentObject || currentGroup);
      gameController.level.copy(currentObject || currentGroup)
    }
  }

  Action {
    id: pasteAction
    text: i18n.t("game-editor.paste")
    enabled: gameController.level.clipperFilled
    shortcut: Shortcut { sequence: "Ctrl+V"; onActivated: pasteAction.trigger() }
    onTriggered: {
      console.log("LevelToolBar: paste", currentObject || currentGroup);
      gameController.level.pasteIn(currentGroup)
    }
  }

  Action {
    id: showAction
    text: i18n.t("game-editor.show")
    shortcut: Shortcut { sequence: "Ctrl+V"; onActivated: showAction.trigger() }
    onTriggered: showClicked(currentObject)
  }

  // END Actions

  Breadcrumbs {
    currentObject: root.currentObject
    currentGroup:  root.currentGroup
    onPathChanged: root.pathChanged(newPath)
  }

  TerminalToolButton {
    iconName: "see"
    action: showAction
    hideText: true
    Layout.alignment: Qt.AlignRight
    Layout.preferredHeight: 35
    Layout.preferredWidth: 40
    visible: currentObject !== null
  }

  TerminalToolButton {
    iconName: "group-add"
    action: addGroupAction
    hideText: true
    Layout.alignment: Qt.AlignRight
    Layout.preferredHeight: 35
    Layout.preferredWidth: 40
    visible: currentObject === null
  }

  TerminalToolButton {
    iconName: "add"
    action: addObjectAction
    hideText: true
    Layout.alignment: Qt.AlignRight
    Layout.preferredHeight: 35
    Layout.preferredWidth: 40
    visible: currentObject === null
  }

  TerminalToolButton {
    iconName: "copy"
    action: copyAction
    hideText: true
    Layout.alignment: Qt.AlignRight
    Layout.preferredHeight: 35
    Layout.preferredWidth: 40
    visible: currentGroup !== gameController.level || currentObject !== null
  }

  TerminalToolButton {
    iconName: "paste"
    action: pasteAction
    hideText: true
    Layout.alignment: Qt.AlignRight
    Layout.preferredHeight: 35
    Layout.preferredWidth: 40
    visible: gameController.level.clipperFilled && currentObject === null
  }

  TerminalToolButton {
    iconName: "trash"
    action: removeAction
    hideText: true
    Layout.alignment: Qt.AlignRight
    Layout.preferredHeight: 35
    Layout.preferredWidth: 40
    visible: (currentObject || currentGroup.parent) !== null
  }
}

