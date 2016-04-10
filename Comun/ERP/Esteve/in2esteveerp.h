#ifndef IN2ESTEVEERP_H
#define IN2ESTEVEERP_H

#include <QObject>
#include <QStringList>
#include <QThread>
#include <QDateTime>
#include <QDate>
#include "in2gestorerp.h"
#include "In2GestorERPDaemon.h"
#include "in2estevepending.h"
#include "In2EsteveFramework.h"


class In2EsteveERP : public QObject
{
    Q_OBJECT
public:
    static In2EsteveERP *request(QObject *parent);
    //explicit In2EsteveERP(QObject *parent = 0);
    QString getItemIn(int item);
    QString getItemOut(int item);
    bool setItemIn(int index,QString item);
    bool setItemOut(int index,QString item);
    void readSGLPending(const QStringList & lst);
    void readSGLError(const QStringList & lst);
    void readSGLFin(const QStringList & lst);
    QStringList checkERP();
    void writeIn2BBDD();
    void SimulatewriteIn2BBDD();
    void processDataIn();
    QStringList execSQL(QString select, bool save);
    void sltWriteAnswerToBBDD(QString state, QString Error);
    bool orderPending();
    QStringList lstOrderPending();
    void loadPendingOrder();
    QString getLastError();
    bool checkCondicionesFisicasEntrada();

signals:
    void SendDebugInfo(unsigned char val,QString txt);
    void sgnERPNewMetrosRef(int metros);
    void sgnERPShowLog();
    void sgnERPWorkLoaded(int mode);
    void sgnERPLaunchEmpalme();
    void sgnERPTrabajoNuevo();
    void sgnMessage(QString  txt);

private:
    In2EsteveERP(QObject *parent = 0);
    QDateTime formatDateTime(QString foo);

public slots:
    void sltFinImpresion();
    void sltFinEmpalmeEntrada();
    void sltFinEmpalmeSalida();

private slots:
    void _sltGenerateReport();
    void _loadDataToMemoryMap();
    void _loadDatum(QStringList lst);
    bool setDataOut(QStringList &data);
    bool setDataIn(const QStringList &data);
    void sltFinDocumentacion(QString name);
    void sltFinFailDocumentacion(QString & txt);
    void sltWriteDatabaseData(QString  lst);
    void saveOrder();
    void deleteOrder();





private:
    static In2EsteveERP *mInstance;
    QStringList m_dataIn;
    QStringList m_dataOut;
    In2GestorERP *m_gestorErp;
    In2GestorERPDaemon *m_ERPdaemon;
    bool m_pendingOrder;
    QString m_lastError;
signals:
    
public slots:
    
};

#endif // IN2ESTEVEERP_H
