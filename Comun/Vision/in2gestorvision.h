#ifndef IN2GESTORVISION_H
#define IN2GESTORVISION_H

#include <QObject>
#include "in2gestorais.h"

#define NO_COMMAND  0
#define VISION_ACK  1
#define VISION_NACK 2
#define LOAD_STRING 3
#define LOAD_FILE   4
#define SAVE_FILE   5
#define STOP_VISION 6
#define VISION_CONFIG 7


class In2GestorVision : public QObject
{
    Q_OBJECT
public:
    static In2GestorVision *request(QObject *parent);    
    bool sendCommand(int command,QString txt);
    bool sendString(QString txt);
    bool isEnabled();
    bool sltLoadWork(QString txt);
    QString lastCommandSent();
    bool isConnected();
    bool sltMapearUnidad();
    void wakeUp();
    void setTryReconn(bool val);
    bool socketConnected();
    bool equipoConnected();
public slots:
    bool connect();
private:
    explicit In2GestorVision(QObject *parent = 0);

private:
    In2GestorAIS *m_gestorAIS;
    QString m_vendor;
    bool m_enabled;
    static In2GestorVision *mInstance;
    int m_lastCommand;
    QString m_strLastCommand;
    bool m_bSocketConnected;
    bool m_bEquipoConnected;

signals:
        void sgnVisionReceived(QString txt);
        void SendDebugInfo(unsigned char val,QString txt);
        void sgnVisionAlarm(QString txt);
        void sgnVisionCommandAnswer(int val,int command);
        void sgnVisionNoComm();
        void sgnSocketDisconnected();
        void sgnSocketConnected();
        void sgnVisionRemoteAlive();
        void sgnVisionRemoteDead();

private slots:
        void sltVisionReceived(QString txt);
        void sltSocketDisconnected();
        void sltSocketConnected();
        void sltVisionRemoteAlive();
        void sltVisionRemoteDead();

};

#endif // IN2GESTORVISION_H
