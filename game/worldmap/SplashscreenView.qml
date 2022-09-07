import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../assets/ui" as UiStyle
import "../../ui"

Image {
  id: root
  property QtObject location

  signal entryPointClicked(string entryPoint)

  source: assetPath + "/locations/" + location.splashscreen.imagePath
  Repeater {
    model: location.splashscreen.entryPoints
    delegate: TerminalButton {
      property string entryPoint: location.splashscreen.entryPoints[index]
      property point position: location.splashscreen.getPositionFor(entryPoint)
      visible: gameManager.currentGame.isGameEditor || gameManager.currentGame.dataEngine.hasLevelBeenVisited(entryPoint)
      text: i18n.t("cities.entryPoints." + entryPoint)
      x: root.width * (position.x / 100) - width / 2
      y: root.height * (position.y / 100) - height / 2
      onClicked: entryPointClicked(entryPoint)
      Connections {
        target: location.splashscreen
        function onEntryPointsChanged() {
          console.log("Coucou petite perruche");
          position = location.splashscreen.getPositionFor(entryPoint)
        }
      }
    }
  }
}
