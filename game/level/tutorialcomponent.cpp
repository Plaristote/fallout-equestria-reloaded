#include "tutorialcomponent.h"
#include "globals.h"
#include "game.h"
#include "game/leveltask.h"
#include <QFile>
#include <QDir>
#include "i18n.h"

static QString tutorialPath(const QString& levelName)
{
  return ASSETS_PATH + "/tutorials/" + levelName;
}

TutorialComponent::TutorialComponent(QObject *parent) : QObject(parent)
{
  QDir directory(getHtmlPath());

  visible = false;
  page = -1;
  connect(this, &TutorialComponent::pageChanged, this, &TutorialComponent::loadPage);
  connect(this, &TutorialComponent::visibleChanged, this, &TutorialComponent::onVisibilityChanged);
  pages = directory.entryList(QStringList() << "*.html", QDir::Files);
}

QString TutorialComponent::getHtmlPath() const
{
  return tutorialPath(parent()->property("name").toString())
       + '/' + I18n::get()->getCurrentLocale();
}

bool TutorialComponent::hasTutorialForLevel(const QString& name)
{
  return QDir(tutorialPath(name)).exists();
}

void TutorialComponent::loadPage()
{
  if (page >= 0 && page < pages.size() && parent())
  {
    QFile file(getHtmlPath() + '/' + pages[page]);

    if (file.open(QIODevice::ReadOnly))
    {
      title = I18n::get()->t("tutorial." + QString::number(page + 1));
      html  = file.readAll();
      emit htmlChanged();
    }
    else
      qDebug() << "Could not open tutorial file" << (ASSETS_PATH + '/' + pages[page]);
  }
  else
    qDebug() << "Cannot load page...";
}

void TutorialComponent::onVisibilityChanged()
{
  Game::get()->getLevel()->getSoundManager()->play("tutorial-show");
}
