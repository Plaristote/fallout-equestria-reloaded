import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

EditorSelectPanel {
  id: root
  property string currentPath
  property string prefix: currentPath.length > 0 ? currentPath + '/' : ''
  property var treeModel: []
  property var treeBranch: ({})

  listItem: treeListItem

  onTreeModelChanged: updateTree(treeModel)
  onPrefixChanged: updateTree(treeModel)
  onTreeBranchChanged: {
    let result = [];
    for (let key in treeBranch) { if (treeBranch[key].type == "folder") result.push(key); }
    for (let key in treeBranch) { if (treeBranch[key].type != "folder") result.push(key); }
    if (currentPath.length > 0) result.unshift("..")
    root.model = result;
  }

  function updateTree(model) {
    let result = {};

    // Looping over all the files
    for (let i = 0 ; i < model.length ; ++i) {
      // Is descendent of the current branch
      if (model[i].startsWith(prefix)) {
        let subpath = model[i].substr(prefix.length);
        // is a folder
        if (subpath.indexOf('/') >= 0) {
          let folder = subpath.substr(0, subpath.indexOf('/'));

          if (result[folder] === undefined)
            result[folder] = { type: 'folder', path: prefix + folder };
        }
        // is not a folder
        else {
          result[subpath] = { type: 'file', path: prefix + subpath };
        }
      } // END is descendent of current branch
    } // END looping over all files
    treeBranch = result;
  }

  function itemClicked(model, isFolder) {
    if (model == "..")
      currentPath = currentPath.substr(0, currentPath.lastIndexOf('/'));
    else if (isFolder)
      currentPath = prefix + model;
    else
      root.currentName = prefix + model;
  }

  Component {
    id: treeListItem
    TerminalButton {
      property bool isFolder: (treeBranch[model] && treeBranch[model].type === "folder") || model === ".."
      property string name: isFolder ? model : model.substr(0, model.lastIndexOf('.'));
      text: isFolder ? `> ${name}` : `- ${name}`
      onClicked: itemClicked(model, isFolder)
      backgroundColor: root.currentName === (prefix + model) || down ? "green" : "transparent"
    }
  }
}
