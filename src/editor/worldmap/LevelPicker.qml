import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import ".."
import "../../ui"

TerminalComboBox {
  property string value
  model: scriptController.getLevels()
  currentIndex: scriptController.getLevels().indexOf(value)
  onCurrentTextChanged: value = currentText
  clip: true
}
