#ifndef  MY_GLOBALS_H
# define MY_GLOBALS_H

# include <QString>
# include <QDir>

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

#endif // MY_GLOBALS_H
