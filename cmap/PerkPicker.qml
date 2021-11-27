import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle
import "../ui"

UiStyle.CustomDialog {
  id: root
  property QtObject characterSheet
  property var perkList: characterSheet.getAvailablePerks()
  property int currentIndex: 0
  property var currentPerk: perkList[currentIndex]
  title: i18n.t("cmap.perk-picker")
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel
  anchors.centerIn: parent
  width: Math.min(parent.width, 1000)
  height: 600
  onAccepted: {
    characterSheet.togglePerk(perkList[currentIndex], true);
    perkList = characterSheet.getAvailablePerks();
    if (characterSheet.availablePerks > 0)
      open();
  }

  RowLayout {
    anchors.fill: parent
    Pane {
      Layout.fillHeight: true
      Layout.preferredWidth: 400
      background: UiStyle.TerminalPane {}
      ColumnLayout {
        anchors.fill: parent
        TerminalLabel {
          text: i18n.t("cmap.available-perks") + `: ${characterSheet.availablePerks}`
        }

        Flickable {
          Layout.fillHeight: true
          Layout.fillWidth: true
          contentHeight: perkColumn.height
          clip: true
          ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

          ColumnLayout {
            id: perkColumn
            spacing: 5
            width: parent.width - 15

            Repeater {
              model: root.perkList.length
              delegate: TerminalButton {
                Layout.fillWidth: true
                backgroundColor: index === root.currentIndex ? "green" : "transparent"
                text: i18n.t(`cmap.${root.perkList[index]}`)
                onClicked: root.currentIndex = index;
              } // END Button
            } // END Repeater
          } // END ColumnLayout
        } // END Flickable
      }
    }

    DescriptionPane {
      id: dialogPanel
      selectedProperty: root.currentPerk ? root.currentPerk : ""
      Layout.fillWidth: true
      Layout.fillHeight: true
    }
  }
}
