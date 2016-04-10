#ifndef IN2CINFAPILLSPENDING_H
#define IN2CINFAPILLSPENDING_H

#include <QObject>
#include <QStringList>
class In2CinfaPillsPending : public QObject
{
    Q_OBJECT
public:
    explicit In2CinfaPillsPending(QObject *parent = 0);

    bool orderPending();
    void deletePending();
    void saveOrder(QStringList &msg);
    QStringList getOrder();

signals:
    
public slots:
    
};

#endif // IN2CINFAPILLSPENDING_H
