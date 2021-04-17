#ifndef  MY_GLOBALS_H
# define MY_GLOBALS_H

# include <QString>
# include <QDir>
# include <QtGlobal>

# ifdef RELEASE_BUILD
#  define ROOT_PATH QString(":/")
#  define FILE_PROTOCOL QString("qrc:/")
# else
#  define ROOT_PATH (QDir(".").absolutePath() + '/')
#  define FILE_PROTOCOL QString("file:")
# endif

# define SCRIPTS_PATH (ROOT_PATH + "scripts/")
# define ASSETS_PATH  (ROOT_PATH + "assets/")
# define SOUNDS_PATH  (ASSETS_PATH + "audio/")

# if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
#  define QML_QLIST_CONSTRUCTOR(type, list) \
   QQmlListProperty<type>(this, list)
# else
#  define QML_QLIST_CONSTRUCTOR(type, list) \
   QQmlListProperty<type>(this, &list)
# endif

#endif // MY_GLOBALS_H
