#ifndef IN2OPCSERVER_H
#define IN2OPCSERVER_H

#include <QThread>

class In2OPCServer : public QThread
{
    Q_OBJECT
public:
    explicit In2OPCServer(QObject *parent = 0);
    void run();
    
signals:

    
public slots:
    
};

#endif // IN2OPCSERVER_H
