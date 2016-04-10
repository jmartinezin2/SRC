#ifndef IN2LOG_H
#define IN2LOG_H

#include <QObject>

class In2Log : public QObject
{
    Q_OBJECT
public:
    explicit In2Log(QObject *parent = 0);
    QStringList execSQL(QString message);
    bool execSQLNoSave(QString message);
    
signals:
    
public slots:
    
};

#endif // IN2LOG_H
