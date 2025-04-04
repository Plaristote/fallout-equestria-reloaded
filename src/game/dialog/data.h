#ifndef  DIALOGDATA_H
# define DIALOGDATA_H

# include "state.h"
# include <QSharedPointer>

class DialogData
{
public:
  void load(const QJsonObject&);
  void save(QJsonObject&) const;
  void reset();

  QSharedPointer<DialogAnswer>    findAnswer(const QString& symbol);
  QSharedPointer<DialogStateData> findState(const QString& symbol);

  bool    isAnswerAvailable(CharacterDialog& dialog, const QString& symbol);
  QString getAnswerText(CharacterDialog& dialog, const QString& symbol);
  QString triggerAnswer(CharacterDialog& dialog, const QString& symbol);
  QString registerAnswer(const QJSValue&);
  const QString& getDefaultEntryPoint() const { return defaultEntryPoint; }
  void           setDefaultEntryPoint(const QString& value) { defaultEntryPoint = value; }

  const QList<QSharedPointer<DialogAnswer>>&    getAnswers() const { return answers; }
  const QList<QSharedPointer<DialogStateData>>& getStates() const { return states; }
  DialogStateData& createNewState(const QString& symbol);
  DialogAnswer&    createNewAnswer(const QString& symbol);
  void             removeState(const QString& symbol);
  void             removeAnswerIfUnreferenced(const QString& symbol);

private:
  QString defaultEntryPoint;
  QList<QSharedPointer<DialogAnswer>>    answers;
  QList<QSharedPointer<DialogStateData>> states;
};


#endif
