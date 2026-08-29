import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/com/planed/foengine/qml/com/planed/foengine/ui/style" as UiStyle
import "qrc:/com/planed/foengine/qml/com/planed/foengine/ui"
import ".."

TerminalComboBox {
  property string value
  model: scriptController.getLevels()
  currentIndex: scriptController.getLevels().indexOf(value)
  onCurrentTextChanged: value = currentText
  clip: true
}
