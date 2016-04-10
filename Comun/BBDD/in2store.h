#ifndef IN2STORE_H
#define IN2STORE_H

#include <QObject>
#include <QString>

struct strctIn2Store{
   QString fecha;
   QString hora;
   QString user;
   QString evento;
   QString codigo;
   QString tarea;

};

class In2Store : public QObject
{
    Q_OBJECT
public:
    explicit In2Store(QObject *parent = 0);
    bool saveMessage(const strctIn2Store & data, QString proceso);
    bool createTable(QString proceso);
    QStringList getAllData(QString tabla);
    QStringList getAllDataFromCode(QString tabla,QString code);
    QStringList getTables();
    int fieldsLastQuery();
    QStringList execSql(QString sql);
    float getMetrosMaquina(QString tabla);
    float getMetrosImpresos(QString tabla);


private:
    QStringList getDataFiltered(QString tabla,QString column,QString filter);
signals:

private:
    int m_nFieldsLastQuery;
    
public slots:
    
};

#endif // IN2STORE_H
