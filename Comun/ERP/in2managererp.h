#ifndef IN2MANAGERERP_H
#define IN2MANAGERERP_H

#include <QObject>
#include "in2configuratorerp.h"
#include "in2gestorerp.h"
#include "in2esteveerp.h"
#include "in2gramarerp.h"
#include "in2erp.h"
#include <QStringList>

class In2ManagerERP : public QObject
{
    Q_OBJECT

private:
    In2ManagerERP(QObject *parent = 0);

public:
    static In2ManagerERP* request(QObject *parent);
    QString getItem(QString item);
    int getItemIndex(QString item);
    QStringList checkERP();
    QStringList execSQL(QString select, bool save);
    void processDataIn();
    QString getClient();
    bool insertCurrentWork();
    bool empalmePermitido();
    QString getValueFromERP(int index);
    bool checkCondicionesFisicasEntrada();
    QString getLastError();
    void setValueERPOut(int index,QString value);
    bool canLoadWork();
    void sltWriteAnswerToBBDD(QString state,QString error);
    bool orderPending();
    QStringList lstOrderPending();
    void loadPendingOrder();
    void writeIn2BBDD();
    void SimulatewriteIn2BBDD();
    void deleteOrden(QString campo,QString valor);
public slots:
    void sltFinImpresion();
    void sltFinEmpalmeEntrada();
    void sltFinEmpalmeSalida();

private:
    In2ConfiguratorERP config;
    QString m_Cliente;
    static In2ManagerERP *mInstance;
    In2EsteveERP *m_SGL;
    In2ERP *m_In2erp;
    In2GramarErp *m_GramarErp;


signals:
    void sgnERPWorkLoaded(int val);
    void SendDebugInfo(unsigned char index,QString txt);
    void sgnERPNewMetrosRef(int x);
    void sgnERPShowLog();
    void sgnERPLaunchEmpalme();
    void sgnERPTrabajoNuevo();
    void sgnMessage(QString txt);
    
public slots:
    
};

#endif // IN2MANAGERERP_H
