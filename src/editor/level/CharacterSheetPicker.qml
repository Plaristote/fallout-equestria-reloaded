import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "../../ui"
import "./"

ResourcePicker {
  id: root
  availableItems: scriptController.getCharacterSheets()

  function startPicking(characterSheet) {
    open();
    currentIndex = availableItems.indexOf(characterSheet);
  }
}
