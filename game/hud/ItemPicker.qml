import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents

Pane {
  id: root
  property QtObject target
  property alias inventory: itemsView.inventory
  property QtObject selectedObject

  background: UiStyle.Pane {}

  signal accepted()
  signal closed()

  Text {
    id: header
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.leftMargin: 10
    text: i18n.t("Pick an item")
    font.family: application.titleFontName
    font.pointSize: 18
    color: "white"
  }

  RowLayout {
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: footer.top }

    GameComponents.InventoryItemsView {
      id: itemsView
      Layout.fillHeight: true
      Layout.preferredWidth: parent.width > 800 ? 400 : 200
      selectedObject: root.selectedObject
      onItemSelected: root.selectedObject = selectedItem
    }

    Loader {
      Layout.fillWidth: true
      sourceComponent: root.selectedObject ? itemPreviewComponent : null
    }
  }

  Row {
    id: footer
    anchors.bottom: parent.bottom
    anchors.right: parent.right

    MenuButton {
      id: useButton
      text: i18n.t("Use")
      visible: selectedObject != null
      onClicked: root.accepted();
    }

    MenuButton {
      id: closeButton
      text: i18n.t("Close")
      onClicked: root.closed();
    }
  }

  Component {
    id: itemPreviewComponent
    InventoryItemPreview {
      model: root.selectedObject
    }
  }
}
