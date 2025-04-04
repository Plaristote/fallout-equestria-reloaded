#ifndef PROMPTDIALOG_H
#define PROMPTDIALOG_H

#include <QObject>
#include <QJSValue>

class PromptDialog : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString title MEMBER title CONSTANT)
  Q_PROPERTY(QStringList options READ getOptions CONSTANT)
public:
  explicit PromptDialog(const QString& title, QJSValue, QObject *parent = nullptr);

  QStringList getOptions() const { return labels; }
  Q_INVOKABLE void pick(int index);

private:
  QString title;
  QStringList labels;
  QList<QJSValue> callbacks;
};

#endif // PROMPTDIALOG_H
