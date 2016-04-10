#ifndef IN2GESTORAIS_H
#define IN2GESTORAIS_H

#include <QObject>
#include <QTcpSocket>
#include <QtNetwork>

#define AIS_ACK  "ACK"
#define AIS_NACK "NACK"
#define AIS_CONFIG "%"
#define AIS_NOSYNC "NO_SYNC"
#define AIS_ERROCR  "Err_OCR"
#define AIS_ERRTEMPMANCHA "Err_temp_mancha"
#define AIS_ERRTEMPCABEZAL "Err_temp_cabezal"

class In2GestorAIS : public QObject
{
    Q_OBJECT
public:
    explicit In2GestorAIS(QObject *parent = 0);
    //~In2GestorAIS();
    bool sendString(QString txt);    
    bool isConnected();
    void setTryReconn(bool val);
    void wakeUp();
    
signals:
    void sgnAISReceived(QString txt);
    void sgnAISNoComm();
    void SendDebugInfo(unsigned char val, QString txt);
    void sgnSocketDisconnected();
    void sgnSocketConnected();
    void sgnRemoteAlive();
    void sgnRemoteDead();
    void sgnVisionText(QString txt);
public slots:
    bool connect();
    bool sltMapearUnidad();
private slots:
        void sltReadSocket();
        void sltTestAnswer();
        void sltSocketConnected();
        void sltSocketDisConnected();
private:
    QTcpSocket m_socket;
    bool m_bWaitAnswer;
    bool m_bConnected;
    bool m_tryRecon;
    QString m_lasCommand;
    QString m_ip;
    QString m_port;

public slots:
    
};

#endif // IN2GESTORAIS_H
