#ifndef IN2BUFFERIMAGES_H
#define IN2BUFFERIMAGES_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include "in2defines.h"

class In2BufferImages : public QObject
{
    Q_OBJECT
public:
    explicit In2BufferImages(QObject *parent = 0);
    struct In2VarImage at(int index);
    void replace(int index, struct In2VarImage & element );
    void clear();
    void append(struct In2VarImage & element);
    int count();

private:
    QList<struct In2VarImage> m_buffer;
    QMutex m_mutex;

signals:
    
public slots:
    
};

#endif // IN2BUFFERIMAGES_H
