#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG_FLAG
#  include <QDebug>
#  define DEBUG(CODE) qDebug() << CODE
#else
#  include <QDebug>
#  define DEBUG(CODE)
#endif



#endif // DEBUG_H
