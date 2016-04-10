#ifndef IN2ESTEVEPENDING_H
#define IN2ESTEVEPENDING_H

#include <QObject>
#include <QStringList>
#include "In2EsteveFramework.h"

class In2EstevePending : public QObject
{
    Q_OBJECT
public:
    explicit In2EstevePending(QObject *parent = 0);
    bool orderPending();
    void deletePending();
    void saveOrder(QStringList &msg);
    QStringList getOrder();

signals:

public slots:
    
};

#endif // IN2ESTEVEPENDING_H
