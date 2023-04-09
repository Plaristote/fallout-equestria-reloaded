#ifndef  JAVACRIPT2I18N_HPP
# define JAVACRIPT2I18N_HPP

# include "game/scriptcontroller.h"
# include <QVariantMap>

QVariantMap fetchScriptVariablesFromTranslation(QString translationPath, QVariantMap variables, ScriptController* script);

#endif
