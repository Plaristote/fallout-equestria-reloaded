import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.2 as BiDialog
import "qrc:/assets/ui" as UiStyle
import "../ui"

Item {
  id: root
  property var animationGroups: animationLibrary.getGroups()
  property alias animationGroup: groupSelect.currentName
  property QtObject spriteAnimation

  onAnimationGroupChanged: {
    spriteGroupLoader.sourceComponent = null;
    spriteGroupLoader.sourceComponent = spriteGroupComponent;
  }

  TextPromptDialog {
    id: newGroupDialog
    title: "New sprite group"
    anchors.centerIn: parent

    onAccepted: {
      animationGroups.push(value)
      animationGroupsChanged();
    }
  }

  BiDialog.FileDialog {
    property var target
    id: filePicker
    title: "Please chose a file"
    folder: rootPath + "assets/sprites"
    nameFilters: ["Image files (*.jpg, *.png, *.webp)"]

    onAccepted: {
      const path = filePicker.fileUrl.toString().replace(/.*\/assets\/sprites\//, "")
      console.log("File selected", path);
      target.text = path;
    }
  }

  RowLayout {
    anchors.fill: parent

    // Groups select
    EditorSelectPanel {
      id: groupSelect
      model: animationGroups
      onNewClicked: newGroupDialog.open()
    }

    Loader {
      id: spriteGroupLoader
      Layout.fillWidth: true
      Layout.fillHeight: true
    }
  } // END RowLayout

  Component {
    id: spriteGroupComponent
    SpriteGroup {
      animationGroup: root.animationGroup
    }
  }
}
