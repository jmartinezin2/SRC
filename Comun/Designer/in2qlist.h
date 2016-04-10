#ifndef IN2QLIST_H
#define IN2QLIST_H

#include <QList>
#include <QMutex>
#include <QMutexLocker>

class In2QList : public QList<struct In2VarImage>
{
    Q_OBJECT
public:
    explicit In2QList(QObject *parent = 0);
    const struct In2VarImage & ts_at(int index);

private:
    QMutex m_mutex;
signals:
    
public slots:
    
};

#endif // IN2QLIST_H
