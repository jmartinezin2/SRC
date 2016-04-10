#ifndef IN2GESTORERP_H
#define IN2GESTORERP_H

#include <QObject>
#include <QStringList>
#include <QVariantList>


//Posibles ERPs
#define ERP_IN2     "ERP_IN2"
#define ERP_ESTEVE  "ERP_ESTEVE"
#define ERP_GRAMAR  "ERP_GRAMAR"

class In2GestorERP : public QObject
{
    Q_OBJECT
public:
    static In2GestorERP *request(QObject *parent);
    QStringList checkERP();
    QStringList execSQL(QString txt, bool save);
    bool execClob(QString txt,QVariantList &lst);
    void writeResultERP();
    void setConnection(QString txt);
    void setUser(QString txt);
    void setPassword(QString txt);
    void setCliente(QString txt);
    void setEnabled(bool val);
    void setDriver(QString txt);
    void setODBCEntry(QString txt);
    void setBBDD(QString txt);
    void setReadBBDD(QString txt);
    void setWriteBBDD(QString txt);
    void setFieldsIn(QString txt);
    void setFieldsOut(QString txt);
    void setTableIn(QString txt);
    void setTableOut(QString txt);
    void setMultipleItems(bool val);
    void setFieldCompo(QString txt);
    void setFieldOT(QString txt);
    void setFieldEstado(QString txt);
    void setFieldError(QString txt);
    void setFieldTipoTarea(QString txt);
    void setMetros(QString txt);



    int getOTFieldIndex();
    int getCompoFieldIndex();
    int getMetrosFieldIndex();

    QString getMetros();
    QString getFieldsIn();
    QString getFieldsOut();
    QString getTableIn();
    QString getTableOut();
    QString getODBCEntry();
    QString getDriver();
    QString getConnection();
    QString getUser();
    QString getPassword();
    QString getCliente();
    QString getBBDD();
    QString getReadBBDD();
    QString getWriteBBDD();
    QString getFieldCompo();
    QString getFieldOT();
    QString getFieldEstado();
    QString getFieldError();
    QString getFieldMsg();
    QString getFieldTipoTarea();

    bool getMultipleItems();
    bool getEnabled();
    QStringList getLastRequest();
    bool getLastQueryResult();

private:
    In2GestorERP(QObject *parent = 0);

signals:
    void In2ERP_SQLData(const QStringList & data);
    void SendDebugInfo(unsigned char val,QString txt);
    void sgnShowInfo();

private:
    static In2GestorERP *mInstance;
    QStringList m_savedRequest;
    QString m_Connection;
    QString m_Driver;
    QString m_ODBCEntry;
    QString m_User;
    QString m_Password;
    QString m_Cliente;
    QString m_BBDD;
    QString m_ReadBBDD;
    QString m_WriteBBDD;
    QString m_fieldsIn;
    QString m_fieldsOut;
    QString m_inTable;
    QString m_outTable;
    QString m_fieldCompo;
    QString m_fieldOT;
    QString m_fieldEstado;
    QString m_fieldMetros;
    QString m_fieldError;
    QString m_fieldTipoTarea;
    QString m_fieldMsg;
    bool m_enabled;
    bool m_bMultipleItems;
    bool m_lastQueryResult;

public slots:
    
};

#endif // IN2GESTORERP_H
