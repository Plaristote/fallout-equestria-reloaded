import QtQuick 2.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../ui"

ColumnLayout {
  property bool editingZone: false
  property QtObject selectedObject: null
  property var displayRoofs: null
  property var displayWalls: null
  property bool hasControlZone: false

  onSelectedObjectChanged: refresh()

  function toggleTile(tileX, tileY) {
    const zone = selectedObject.controlZone;

    zone.offset = selectedObject.position;
    if (zone.isInside(tileX, tileY, selectedObject.floor))
    {
      console.log("Removing tile", tileX, ",", tileY, "from", selectedObject, selectedObject.position, "control zone");
      zone.removePosition(Qt.point(tileX, tileY));
    }
    else
    {
      console.log("Adding tile", tileX, ",", tileY, "to", selectedObject, selectedObject.position, "control zone");
      zone.addPosition(Qt.point(tileX, tileY));
    }
  }

  function refresh() {
    hasControlZone = selectedObject !== null && selectedObject.controlZone !== null;
    if (hasControlZone)
      pathBlockedInput.checked = selectedObject.zoneBlocked;
  }

  Connections {
    target: selectedObject
    function onControlZoneChanged() { refresh(); }
  }

  Action {
    id: toggleZoneEditAction
    property bool backupShowRoof
    property bool backupShowWalls
    onTriggered: {
      if (editingZone) {
        displayRoofs.checked = backupShowRoof;
        displayWalls.checked = backupShowWalls;
      } else {
        backupShowRoof  = displayRoofs.checked;
        backupShowWalls = displayWalls.checked;
      }
      editingZone = !editingZone
    }
  }

  spacing: 5

  TerminalLabel { text: `> ${i18n.t("game-editor.levels.control-zone")}`; font.pointSize: 13 }

  TerminalButton {
    text: i18n.t("game-editor.levels.control-zones.add")
    visible: !hasControlZone
    width: parent.width
    height: 20
    onClicked: selectedObject.addControlZone()
  }

  TerminalButton {
    text: i18n.t("game-editor.levels.control-zones." + (!canvas.editingZone ? "edit" : "end-edit"))
    visible: hasControlZone
    width: parent.width
    height: 20
    action: toggleZoneEditAction
  }

  TerminalCheckBox {
    id: pathBlockedInput
    text: i18n.t("game-editor.levels.control-zones.blocks-path")
    visible: hasControlZone
    checked: selectedObject && selectedObject.zoneBlocked
    onCheckedChanged: { selectedObject.zoneBlocked = checked; }
  }

  TerminalButton {
    text: i18n.t("game-editor.levels.control-zones.remove")
    visible: hasControlZone
    width: parent.width
    height: 20
    onClicked: {
      editingZone = false;
      selectedObject.removeControlZone();
    }
  }
}
