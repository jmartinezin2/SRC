#ifndef IN2PING_H
#define IN2PING_H

#include <QObject>

class In2Ping : public QObject
{
    Q_OBJECT
public:
    explicit In2Ping(QObject *parent = 0);
    bool ping(QString host);
    
signals:
    
public slots:
    
};

#endif // IN2PING_H
