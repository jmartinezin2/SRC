#ifndef WenglorMessage_H
#define WenglorMessage_H

#include <QObject>
#include <QByteArray>
#include <QMap>


//Coleccion de errores
enum MONDIAMETRO_MESSAGE_ERROR{
    MONDIAMETRO_NOT_ERROR=0,
    MONDIAMETRO_ERROR_SYN,
    MONDIAMETRO_ERROR_SIZE
};


class WenglorMessage : public QObject
{
    Q_OBJECT
public:
    WenglorMessage(QObject *parent = 0);
    WenglorMessage(QObject *parent,const QByteArray &p);
    QByteArray CreateMessageWithData(QByteArray &d);
    QByteArray message();
    QByteArray head();
    QByteArray data();
    char getLastError();
    int isValidMessage();
    void loadMessage(const QByteArray & p);
    char getID();
    void SetUnknownMessage();
private:
    void SetSyn();


private:
    QByteArray m_data;
    char m_lastError;
signals:
    
public slots:
    
};

#endif // WenglorMessage_H
