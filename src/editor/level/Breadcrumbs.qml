import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../ui"

Flickable {
  property QtObject currentObject
  property QtObject currentGroup
  property var crumbs: (currentObject || currentGroup).path.split('.')
  property bool hasCrumbs: crumbs.length > 0 && crumbs[0].length > 0
  property color passiveColor: "lightgray"
  property color activeColor: "white"

  signal pathChanged(string newPath)

  clip: true
  contentWidth: breadcrumbsRow.width
  Layout.preferredHeight: breadcrumbsRow.height
  Layout.fillWidth: true
  onCrumbsChanged: contentX = contentWidth - width

  Row {
    id: breadcrumbsRow
    spacing: 2

    Text {
      text: '»'
      color: "white"
    }

    TerminalButton {
      text: 'root'
      textColor: hasCrumbs ? passiveColor : activeColor
      onClicked: pathChanged("")
    }

    Text {
      text: '»'
      visible: hasCrumbs
      color: "white"
    }

    Repeater {
      model: hasCrumbs ? crumbs : []
      delegate: Row {
        property bool isCurrentItem: index + 1 === crumbs.length
        spacing: breadcrumbsRow.spacing
        TerminalButton {
          text: crumbs[index]
          textColor: isCurrentItem ? activeColor : passiveColor
          onClicked: pathChanged(crumbs.slice(0, index + 1).join('.'))
        }
        Text {
          text: '»'
          visible: !isCurrentItem
          color: "white"
        }
      }
    }
  }
}
