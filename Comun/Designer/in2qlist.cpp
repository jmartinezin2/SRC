#include "in2qlist.h"
In2QList::In2QList(QObject *parent) :
    QList<struct In2VarImage>(parent)
{
}
/** Función thread safe de QList.at()*/
const struct In2VarImage & In2QList::ts_at(int index){
    QMutexLocker(&m_mutex);
    struct In2VarImage rtn=this->at(index);
    return rtn;
}
