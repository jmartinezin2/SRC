#ifndef IN2COUNTERS_H
#define IN2COUNTERS_H

#include <QObject>
#include "GestorDB_SQLITE.h"
#include <QDateTime>
#include "In2GestorDB.h"


#define IN2RECORD_CAMPOS 7

struct In2counterRecord{
    QString m_path;
    QString m_elapsedTime;
    QString m_PrintDate;
    QString m_InitDate;
    QString m_meters;
    QString m_count;
    QString m_finishComment;
    QString m_OT;
};

class In2Counters : public QObject
{
    Q_OBJECT
public:
    explicit In2Counters(QObject *parent = 0);
    ~In2Counters();
    bool addRecord(QString  path,int time,int meters,int count,QString  txt,QString OT);
    QStringList getRecord(QString query);
    int getTotalMeters();
    int getTotalPrinted();

private:
    In2GestorDB m_db;


signals:

    void SendDebugInfo(unsigned char code, QString msg);

public slots:
    
};

#endif // IN2COUNTERS_H
