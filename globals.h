#ifndef  MY_GLOBALS_H
# define MY_GLOBALS_H

# include <QString>

# ifdef RELEASE_BUILD
#  define ROOT_PATH QString(":/")
# else
#  define ROOT_PATH QString("./")
# endif

# define SCRIPTS_PATH (ROOT_PATH + "scripts/")
# define ASSETS_PATH  (ROOT_PATH + "assets/")

#endif // MY_GLOBALS_H
