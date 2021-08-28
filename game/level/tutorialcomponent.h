#ifndef TUTORIALCOMPONENT_H
#define TUTORIALCOMPONENT_H

#include <QObject>

class TutorialComponent : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool enabled MEMBER enabled NOTIFY enabledChanged)
  Q_PROPERTY(bool visible MEMBER visible NOTIFY visibleChanged)
  Q_PROPERTY(int page MEMBER page NOTIFY pageChanged)
  Q_PROPERTY(int maxPage READ maxPage CONSTANT)
  Q_PROPERTY(QString title MEMBER title NOTIFY htmlChanged)
  Q_PROPERTY(QString html MEMBER html NOTIFY htmlChanged)
public:
  explicit TutorialComponent(QObject* parent = nullptr);

  static bool hasTutorialForLevel(const QString&);

  int maxPage() const { return pages.size() - 1; }

signals:
  void enabledChanged();
  void visibleChanged();
  void pageChanged();
  void htmlChanged();

private slots:
  void loadPage();
  void onVisibilityChanged();

private:
  QString getHtmlPath() const;

  bool    visible = false;
  bool    enabled = true;
  int     page;
  QString html, title;
  QStringList pages;
};

#endif // TUTORIALCOMPONENT_H
