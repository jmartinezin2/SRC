#ifndef IN2CONFIGURATORERP_H
#define IN2CONFIGURATORERP_H

#include <QObject>

#define ERP_CONNECTION  "Connection"
#define ERP_ID          "ID"
#define ERP_USER        "User"
#define ERP_PASSWORD    "Password"
#define ERP_CLIENTE     "Cliente"
#define ERP_ENABLED     "Enabled"
#define ERP_FILE_ERP    "In2ERP.ini"
#define ERP_ERP         "ERP"
#define ERP_DRIVER      "Driver"
#define ERP_ODBC_ENTRY  "ODBC_Entry"
#define ERP_BBDD        "BBDD"
#define ERP_READ_BBDD   "Read_BBDD"
#define ERP_WRITE_BBDD   "Write_BBDD"
#define ERP_FIELDS_BBDD_IN "FieldsIn"
#define ERP_FIELDS_BBDD_OUT "FieldsOut"
#define ERP_TABLE_IN    "InTable"
#define ERP_TABLE_OUT   "OutTable"
#define ERP_MULTIPLE_ITEMS "Multiple_Items"
#define ERP_FIELD_COMPO     "FieldCompo"
#define ERP_FIELD_OT        "FieldOT"
#define ERP_FIELD_METROS    "FieldMetros"
#define ERP_FIELD_ESTADO    "FieldEstado"
#define ERP_FIELD_ERROR    "FieldError"
#define ERP_FIELD_TIPO_TAREA "FieldTipoTarea"
#define ERP_FIELD_LOTE       "FieldLote"
#define ERP_FIELD_FCHCAD    "FieldFchCad"
#define ERP_FIELD_FCHFAB    "FieldFchFab"
#define ERP_FIELD_MSG       "FieldMsg"
#define ERP_INTERFILE        "InterFile"
#define ERP_INTERFIELDS_IN   "InterFieldsIn"



class In2ConfiguratorERP : public QObject
{
    Q_OBJECT
public:
    explicit In2ConfiguratorERP(QObject *parent = 0);
    bool readConfig();
    QString getConnection();
    QString getUser();
    QString getPassword();
    QString getClient();
    QString getDriver();
    QString getODBCEntry();
    bool getEnabled();
    QString getBBDD();
    QString getReadBBDD();
    QString getWriteBBDD();
    QString getFieldsIn();
    QString getFieldsOut();
    QString getInTable();
    QString getOutTable();
    bool getMultipleItems();
    QString getFieldCompo();
    QString getFieldOT();
    QString getFieldMetros();
    QString getFieldEstado();
    QString getFieldError();
    QString getFieldTipoTarea();
    QString getFieldLote();
    QString getFieldFchCad();
    QString getFieldFchFab();
    QString getFieldMsg();
    int getCompoFieldIndex();
    int getOTFieldIndex();
    int getMetrosFieldIndex();
    int getFchCadFieldIndex();
    int getLoteFieldIndex();
    QString getID();
    QString getInterFile();
    QString getInterFields();


signals:

private:
    QString m_connection;
    QString m_user;
    QString m_password;
    QString m_cliente;
    QString m_Driver;
    QString m_ODBCEntry;
    QString m_BBDD;
    QString m_readBBDD;
    QString m_writeBBDD;
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
    QString m_fieldFchCad;
    QString m_fieldLote;
    QString m_fieldFchFab;
    QString m_fieldMsg;
    QString m_interFile;
    QString m_interFieldsIn;


    bool m_enabled;
    bool m_bMultipleItems;
    QString m_ID;


    
public slots:
    
};

#endif // IN2CONFIGURATORERP_H
