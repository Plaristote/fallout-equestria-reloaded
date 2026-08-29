import QtQuick 2.15
import "qrc:/com/planed/foengine/qml/com/planed/foengine/ui"

TextPromptDialog {
  required property var tree
  readonly property string path: tree.prefix + value
  readonly property string jsonPath: path + ".json"
}
