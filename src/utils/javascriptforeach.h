#ifndef  JAVASCRIPTFOREACH_H
# define JAVASCRIPTFOREACH_H

# include <functional>
# include <QJSValue>

void forEach(QJSValue array, std::function<void (QJSValue)> callback);

#endif
