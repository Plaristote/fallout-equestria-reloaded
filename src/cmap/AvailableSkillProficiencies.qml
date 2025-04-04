import QtQuick 2.12

AvailableSkillPoints {
  label: i18n.t("cmap.available-skill-proficiencies")
  value: characterSheet.maxProficiencies - characterSheet.proficiencies.length
}
