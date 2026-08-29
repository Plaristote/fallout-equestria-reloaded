import QtQuick 2.12
import QtQuick.Controls 2.12

Action {
  id: action
  shortcut: Shortcut { sequence: "Esc"; onActivated: action.trigger() }
}
