import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
  id: root
  property QtObject level
  property alias back:               backAction
  property alias previousTarget:     previousTargetAction
  property alias nextTarget:         nextTargetAction
  property alias passTurn:           passTurnAction
  property alias openMenu:           openMenuAction
  property alias openInventory:      openInventoryAction
  property alias openSkilldex:       openSkilldexAction
  property alias openSpellbook:      openSpellbookAction
  property alias openCharacterSheet: openCharacterSheetAction
  property alias openPipboy:         openPipboyAction

  signal backTriggered()
  signal previousTargetTriggered()
  signal nextTargetTriggered()
  signal menuTriggered()
  signal inventoryTriggered()
  signal skilldexTriggered()
  signal spellbookTriggered()
  signal debugModeTriggered()
  signal helpTriggered()

  Connections {
    target: gamepad
    enabled: root.enabled
    function onBackClicked() { backAction.trigger() }
    function onSkilldexClicked() { openSkilldexAction.trigger() }
    function onInventoryClicked() { openInventoryAction.trigger() }
    function onStartClicked() { openMenuAction.trigger() }
    function onSelectClicked() { helpAction.trigger() }
    function onLeftClicked() { previousTargetAction.trigger() }
    function onRightClicked() { nextTargetAction.trigger() }
    function onBottomClicked() { openPipboyAction.trigger() }
    function onUpClicked() { openCharacterSheetAction.trigger() }
  }

  Action {
    id: backAction
    enabled: root.enabled
    shortcut: Shortcut {
      sequence: "Esc"
      enabled: backAction.enabled
      onActivated: backAction.trigger()
    }
    onTriggered: backTriggered()
  }

  Action {
    id: helpAction
    enabled: root.enabled
    shortcut: Shortcut {
      sequence: "F1"
      enabled: helpAction.enabled
      onActivated: helpAction.trigger()
    }
    onTriggered: helpTriggered()
  }

  Action {
    id: passTurnAction
    enabled: root.enabled && level
    onTriggered: level.passTurn(level.player)
  }

  Action {
    id: previousTargetAction
    enabled: root.enabled
    shortcut: Shortcut {
      sequence: "Ctrl+Left"
      enabled: previousTargetAction.enabled
      onActivated: previousTargetAction.trigger()
    }
    onTriggered: previousTargetTriggered()
  }

  Action {
    id: nextTargetAction
    enabled: root.enabled
    shortcut: Shortcut {
      sequence: "Ctrl+Right"
      enabled: nextTargetAction.enabled
      onActivated: nextTargetAction.trigger()
    }
    onTriggered: nextTargetTriggered()
  }

  Shortcut {
    sequence: "F2"
    enabled: root.enabled
    onActivated: debugModeTriggered()
  }

  Action {
    id: openMenuAction
    enabled: root.enabled
    onTriggered: menuTriggered()
  }

  Action {
    id: openInventoryAction
    enabled: root.enabled
    shortcut: Shortcut {
      sequence: "i"
      enabled: openInventoryAction.enabled
      onActivated: openInventoryAction.trigger()
    }
    onTriggered: inventoryTriggered()
  }

  Action {
    id: openSkilldexAction
    enabled: root.enabled && level
    shortcut: Shortcut {
      sequence: "s"
      enabled: openSkilldexAction.enabled
      onActivated: openSkilldexAction.trigger()
    }
    onTriggered: skilldexTriggered()
  }

  Action {
    id: openSpellbookAction
    enabled: root.enabled && level
    shortcut: Shortcut {
      sequence: "z"
      enabled: openSpellbookAction.enabled
      onActivated: openSpellbookAction.trigger()
    }
    onTriggered: spellbookTriggered()
  }

  Action {
    id: openCharacterSheetAction
    enabled: root.enabled
    shortcut: Shortcut {
      sequence: "o"
      enabled: openCharacterSheetAction.enabled
      onActivated: openCharacterSheetAction.trigger()
    }
    onTriggered: {
      if (level)
        level.mouseMode = 0;
      application.pushView("game/CharacterView.qml", {gameController: gameManager.currentGame})
    }
  }

  Action {
    id: openPipboyAction
    enabled: root.enabled
    shortcut: Shortcut {
      sequence: "p"
      enabled: openPipboyAction.enabled
      onActivated: openPipboyAction.trigger()
    }
    onTriggered: {
      if (level)
        level.mouseMode = 0;
      application.pushView("game/PipBoy.qml");
    }
  }
}
