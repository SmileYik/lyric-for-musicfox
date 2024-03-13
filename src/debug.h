#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG_FLAG
#  define DEBUG qDebug()
#else
#  include <QStringList>
#  define DEBUG QStringList()
#endif



#endif // DEBUG_H
