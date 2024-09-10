import QtQuick 2.15
import "../ui"

TextPromptDialog {
  required property var tree
  readonly property string path: tree.prefix + value
  readonly property string jsonPath: path + ".json"
}
