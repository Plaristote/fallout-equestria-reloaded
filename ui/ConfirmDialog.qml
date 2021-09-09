import QtQuick 2.15
import QtQuick.Controls 2.15 as QuickControls
import QtQuick.Layouts 1.12
import "../assets/ui" as UiStyle

UiStyle.CustomDialog {
  property string text;
  id: root
  modal: true
  standardButtons: QuickControls.Dialog.Ok | QuickControls.Dialog.Cancel

  QuickControls.Label {
    text: i18n.t("are-you-sure")
    color: "white"
    font.family: application.titleFontName
    font.pixelSize: 16
  }
}
