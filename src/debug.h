#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG_FLAG
#  include <QDebug>
#  define DEBUG(CODE) qDebug() << CODE
#  define DCODE(CODE) {CODE;}
#else
#  include <QDebug>
#  define DEBUG(CODE)
#  define DCODE(CODE)
#endif



#endif // DEBUG_H
