#include "javascript2i18n.hpp"
#include "i18n.h"

QVariantMap fetchScriptVariablesFromTranslation(QString translationPath, QVariantMap variables, ScriptController* script)
{
  if (script)
  {
    QString base = I18n::get()->t(translationPath);
    QRegularExpression pattern("\\{\\{([a-zA-Z]+[a-zA-Z0-9]*)\\}\\}");
    QRegularExpressionMatch match;

    match = pattern.match(base);
    while (match.hasMatch())
    {
      QString  symbol = match.captured(1);
      QJSValue value  = script->property(symbol);
      QString  replacement = symbol;

      if (!value.isNull() && !value.isUndefined())
        variables[symbol] = value.toString();
      match = pattern.match(base, match.capturedEnd());
    }
  }
  return variables;
}
