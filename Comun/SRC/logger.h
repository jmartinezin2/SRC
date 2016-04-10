#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

//Interfaz de salida
#define LOGGER_USER_GRANTED 0
#define LOGGER_USER_NOT_GRANTED 1
#define LOGGER_USER_INCLUDED 2
#define LOGGER_USER_NOT_INCLUDED 3
#define LOGGER_NO_INIT 4
#define LOGGER_NO_CONNECTION 5


class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);
    int TryConnection(QString name, QString Password);
    void setHostName(const QString & txt);
    void setDN(const QString & txt);
    void setFilter(const QString & txt);
    void setLocalHost(const QString & txt);
signals:

private:
    QString m_hostName;
    QString m_pMyDN;
    QString m_filter;
    QString m_localHost;

public slots:
    
};

#endif // LOGGER_H
