#ifndef IN2GESTORLAMPARA_H
#define IN2GESTORLAMPARA_H

#include <QObject>
#include <QVector>
#include <QMap>
#include "clienteopc.h"


#define LAMPARA_MAXPOS 16
#define LAMPARA_MINPOS 0
#define LAMPARA_REPOSO 5


#define DEFAULT_INTENSIDAD_TABLE "DefaultInt"
#define DEFAULT_POSICION_TABLE "DefaultPos"


class In2GestorLampara : public QObject
{
    Q_OBJECT
public:
    static In2GestorLampara *request(QObject *parent);
    void newSpeed(int value);
    bool loadPosTable(QString table);
    bool loadIntTable(QString table);
    void resetPos();
    void JogMasOn();
    void JogMasOff();
    void JogMenosOn();
    void JogMenosOff();
    int  getPos();    
    void setPos(int pos);    
    void resetLampara();
    QMap<int,int> getCurrentIntTable();
    QMap<int,int> getCurrentPosTable();
    bool isReadyToPrint();
    bool intDinamica();
    bool posDinamica();
    bool enabled();
    void moveToInit();
    void moveToReposo();
    void setMoving(bool val);
    int vendor();


public slots:
    void init();
    void sltAckFromOPC(int item,QVariant value);

private slots:
    void _init1();
    void _resetLampara1();
    void sltSendNewPosicion(int pos);    
    void sltTestSetUp();
private:
    In2GestorLampara(QObject *parent = 0);
    void sltLoadPosicion(int speed);
    void sltLoadIntensidad(int speed);
    bool isReadyToMove();
    void sendSignal(int item,QVariant value);

signals:
    void SendDebugInfo(unsigned char val, QString txt);
    void sgnNewDataLampara(int item, QVariant value);

private:
    static In2GestorLampara *mInstance;
    bool m_enabled;
    bool m_bPosDinamica;
    bool m_bIntDinamica;
    int m_prevSpeed;
    int m_vendor;
    QMap<int,int> m_Posicion;
    QMap<int,int> m_Intensidad;
    int m_pendingPos;
    int m_prevPos;
    bool m_bMoving;

public slots:
};

#endif // IN2GESTORLAMPARA_H
