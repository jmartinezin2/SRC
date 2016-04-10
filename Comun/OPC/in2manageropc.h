#ifndef IN2MANAGEROPC_H
#define IN2MANAGEROPC_H

#include <QObject>

class In2ManagerOPC : public QObject
{
    Q_OBJECT
public:
    explicit In2ManagerOPC(QObject *parent = 0);
    void timedEscribeDato(int ReferenciaItem,QVariant valor);
    void timedExtEscribeDato(int delay,int ReferenciaItem,QVariant valor);
    void Init();
    QVariant returnOPCHandle(int ReferenciaItem);

private:
    void setNewItem(struct OPC_Item *p,QString variable,QString name);
    struct OPC_Item * ReturnOPCItem(int ReferenciaItem);

signals:
    
public slots:
    
};

#endif // IN2MANAGEROPC_H
