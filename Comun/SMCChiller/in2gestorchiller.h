#ifndef IN2GESTORCHILLER_H
#define IN2GESTORCHILLER_H

#include <QObject>
#include "in2smccontrol.h"
#include "puertoserie.h"
#include "gestormodbus.h"

#include <QVector>
#include <QByteArray>

#define FLAG_ESTADO_CHILLER     1
#define FLAG_CHILLER_OK         2
#define FLAG_COMCHILLER_OK      3
#define MAXCHILLERS             1

//Estados a informar
enum CHILLERSTATES{
    CHILLER_OK=0,
    CHILLER_INIT_ERROR=1,
    CHILLER_COMERROR=2
};

//Comunicaciones
#define CHILLER_COMNOK 0
#define CHILLER_COMOK 1
#define CHILLER_COMPENDING 2


class In2GestorChiller : public QObject
{
    Q_OBJECT

public :
    static In2GestorChiller *request(QObject *parent,int equipos);


    void Init();
    void SetFlag(char flag,char value);
    char GetFlag(char flag);
    void InitEquipo(int x);
    void StopEquipo(int x);

    void setSetpoint(int x,quint16 value);
    void setStatus(int x,quint16 value);
    void setAlarm1(int x,quint16 value);
    void setAlarm2(int x,quint16 value);
    void setAlarm3(int x,quint16 value);
    void setReady(int x,bool value);

    int numEquipos();
    void setDischargePressure(int index,quint16 valor);
    void setDischargeTemp(int index,quint16 valor);
    quint16 getDischargePressure(int index);
    quint16 getDischargeTemp(int index);
    quint16 getDischargeSetTemp(int index);
    quint16 getStatus(int x);
    quint16 getAlarm1(int x);
    quint16 getAlarm2(int x);
    quint16 getAlarm3(int x);
    bool getReady(int x);

    QString getModeDescription(int x);
    QString getStatusDescription(int index);
    QString getAlarm1Description(int x);
    QString getAlarm2Description(int x);
    QString getAlarm3Description(int x);
    bool getTemperatureChange(int x);
    bool getPressureChange(int x);
    bool getReadyChange(int x);
    bool getSetpointChange(int x);
    bool getStatusChange(int x);
    bool getAlarm1Change(int x);
    bool getAlarm2Change(int x);
    bool getAlarm3Change(int x);
    void setTemperatureChange(int x,bool valor);
    void setPressureChange(int x,bool valor);
    void setReadyChange(int x,bool valor);
    void setSetpointChange(int x,bool valor);
    void setStatusChange(int x,bool valor);
    void setAlarm1Change(int x,bool valor);
    void setAlarm2Change(int x,bool valor);
    void setAlarm3Change(int x,bool valor);
    unsigned char getMode(int index);
    void setMode(int index,unsigned char valor);
    bool isReadyToPrint();
    bool isEnabled();
    bool isAutoStart();
    bool getPortState();

private:
    In2GestorChiller(QObject *parent = 0,int equipos=0);
    GestorModbus *MiGestorModbus;
    static In2GestorChiller *mInstance;
    QVector<In2SMCControl*> m_equipos;
    short m_numEquipos;
    QString m_Descripcion;
    char m_FlagEstado;
    char m_FlagChillerOK;
    char m_FlagComunicacionChillerOK;
    char m_FlagInitError;
    bool estadoPuerto;
/*    double Discharge_SetTemp[MAXCHILLERS];
    double Discharge_Temp[MAXCHILLERS];
    double Discharge_Pr[MAXCHILLERS];*/
    //int error[MAXCHILLERS];
    unsigned char m_Modes[MAXCHILLERS];

    quint16 sDischarge_SetTemp[MAXCHILLERS];
    quint16 sDischarge_Temp[MAXCHILLERS];
    quint16 sDischarge_Pr[MAXCHILLERS];
    quint16 status[MAXCHILLERS];
    quint16 alarm1[MAXCHILLERS];
    quint16 alarm2[MAXCHILLERS];
    quint16 alarm3[MAXCHILLERS];
    bool readyToPrint[MAXCHILLERS];

    bool temperatureChange[MAXCHILLERS];
    bool pressureChange[MAXCHILLERS];
    bool readyChange[MAXCHILLERS];
    bool setpointChange[MAXCHILLERS];
    bool statusChange[MAXCHILLERS];
    bool alarm1Change[MAXCHILLERS];
    bool alarm2Change[MAXCHILLERS];
    bool alarm3Change[MAXCHILLERS];

    bool m_bEnabled;
    int m_iPuerto;
    int m_bAutoStart;


private slots:
    void filterPresion(int x);
    void filterStatus(int x);
    void filterAlarm1(int x);
    void filterAlarm2(int x);
    void filterAlarm3(int x);
    void filterCambioEstado(int x);
    void filterTemperatura(int x);
    void sltGetFluidicData();
    void sltOpenModbusPort(QString port);
    void sltCloseModbusPort();
    void _sltInitDefaultChiller();

public slots:
    void sltMonitoring();
    void sltNewSetpointValue(int equipo,double valor);
    void sltNewDataAvailable(QByteArray datos);

    void sltRunChiller(int x);
    void sltStopChiller(int x);
    void sltNewDataToSend(QByteArray datos);

    void sltModbusDataSent(QByteArray data);
    void sltModbusDataReceived(QByteArray data);

    QLabel *ShowMensaje(QString texto);

private:


signals:
    void ChillerStateNewValue(char value);
    void SendDebugInfo(unsigned char level,const QString &txt);
    void ChillerError(const QString &txt);
    void sglCambioTemperatura(int equipo);
    void sglCambioPresion(int equipo);
    void sglCambioSetpoint(int equipo);
    void sglCambioStatus(int equipo);
    void sglCambioAlarm1(int equipo);
    void sglCambioAlarm2(int equipo);
    void sglCambioAlarm3(int equipo);
    void sglCambioReady(int equipo);
    void sglNewReceivedMessage(QByteArray data);
    void sglNewSentMessage(QByteArray data);
    void sglEstadoPuertoModbus(bool estado);
    void sgnChillerCom(int value);

public slots:

};

#endif // IN2GESTORCHILLER_H
