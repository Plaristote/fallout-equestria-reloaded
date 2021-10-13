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
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel
  anchors.centerIn: parent
  width: 800
  height: 600
  onAccepted: {
    characterSheet.perks.push(perkList[currentIndex]);
    characterSheet.availablePerks--;
    if (characterSheet.availablePerks > 0)
      open();
  }

  RowLayout {
    anchors.fill: parent
    Pane {
      Layout.fillHeight: true
      Layout.preferredWidth: 230
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
            width: parent.width

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
      selectedProperty: root.perkList[root.currentIndex]
      Layout.fillWidth: true
      Layout.fillHeight: true
    }
  }
}
