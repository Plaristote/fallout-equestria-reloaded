import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  id: root
  property QtObject inventory
  property string currentItemType

  title: i18n.t("game-editor.inventories.add-item")
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  width: 500
  GridLayout {
    width: parent.width
    columns: 2

    CustomLabel { text: i18n.t("game-editor.filter") }
    CustomTextField {
      id: filterInput
      text: ""
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }

    UiStyle.TerminalPane {
      Layout.columnSpan: 2
      Layout.fillWidth: true
      Layout.preferredHeight: 400
      CustomFlickable {
        clip: true
        contentHeight: itemGrid.height
        anchors.fill: parent
        anchors.margins: 5
        Grid {
          id: itemGrid
          width: parent.width
          columns: parent.width / 105
          spacing: 5
          Repeater {
            model: itemLibrary.getObjectList()
            delegate: Rectangle {
              property string itemType: itemLibrary.getObjectList()[index]
              property string iconPath: itemLibrary.getIcon(itemType)
              border.width: currentItemType === itemType ? 1 : 0
              border.color: "green"
              color: "transparent"
              width: 100; height: 100
              visible: filterInput.length === 0 || itemType.indexOf(filterInput.text) >= 0
              clip: true
              Image {
                anchors.centerIn: parent
                source: `${assetPath}icons/${iconPath}`
                fillMode: Image.PreserveAspectCrop
                width: 100; height: 100
              }
              Text {
                text: itemType
                color: "green"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                style: Text.StyledText
              }
              MouseArea {
                anchors.fill: parent
                onClicked: root.currentItemType = itemType
                onDoubleClicked: root.accept()
              }
            }
          }
        }
      }
    }

    CustomLabel { text: i18n.t("game-editor.items.quantity") }
    CustomTextField {
      id: newItemQuantityInput
      text: "1"
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }
  }
  onAccepted: {
    if (itemLibrary.getObjectList().indexOf(currentItemType) >= 0)
      inventory.addItemOfType(currentItemType, parseInt(newItemQuantityInput.text));
  }
}
