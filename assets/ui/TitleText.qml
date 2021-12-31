import QtQuick 2.15

Text {
  property color outline

  color: "black"
  style: Text.Outline
  styleColor: outline
  font.family: application.titleFontName
}
