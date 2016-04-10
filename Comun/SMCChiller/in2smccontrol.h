#ifndef IN2SMCCONTROL_H
#define IN2SMCCONTROL_H

#include <QObject>
#include "gestormodbus.h"


#define SMC_ST_FLAG_RUN                      0x0001
#define SMC_ST_FLAG_OP_STOP_ALARM            0x0002
#define SMC_ST_FLAG_OP_CONT_ALARM            0x0004
#define SMC_ST_FLAG_UNUSED1                  0x0008
#define SMC_ST_FLAG_PRESS_UNIT               0x0010
#define SMC_ST_FLAG_REMOTE_STATUS            0x0020
#define SMC_ST_FLAG_UNUSED2                  0x0040
#define SMC_ST_FLAG_UNUSED3                  0x0080
#define SMC_ST_FLAG_UNUSED4                  0x0100
#define SMC_ST_FLAG_TEMP_READY               0x0200
#define SMC_ST_FLAG_TEMP_UNIT                0x0400
#define SMC_ST_FLAG_RUN_TIMER                0x0800
#define SMC_ST_FLAG_STOP_TIMER               0x1000
#define SMC_ST_FLAG_RESET_AFTER_FAILURE      0x2000
#define SMC_ST_FLAG_ANTI_FREEZING            0x4000
#define SMC_ST_FLAG_AUTO_FILLING             0x8000

#define SMC_SERIAL_READ_MULTIPLE        0x3033
#define SMC_SERIAL_WRITE                0x3036
#define SMC_SERIAL_WRITE_MULTIPLE       0x3040
#define SMC_SERIAL_READ_WRITE_MULTIPLE  0x3047

#define SMC_SERIAL_TEMPERATURE_ADD      0x30303030
#define SMC_SERIAL_PRESSURE_ADD         0x30303032
#define SMC_SERIAL_RESISTIVITY_ADD      0x30303033
#define SMC_SERIAL_STATUS_ADD           0x30303034
#define SMC_SERIAL_ALARM1_ADD           0x30303035
#define SMC_SERIAL_ALARM2_ADD           0x30303036
#define SMC_SERIAL_ALARM3_ADD           0x30303037
#define SMC_SERIAL_SETPOINT_ADD         0x30303042
#define SMC_SERIAL_COMMAND_ADD          0x30303043

#define FLAG_ERROR_INICIANDO 1

#define SMC_MODE_Stopped 0
#define SMC_MODE_Started 1

#define SMC_CMD_Stop     0
#define SMC_CMD_Start    1


class In2SMCControl : public QObject
{
    Q_OBJECT
public:
    explicit In2SMCControl(QObject *parent = 0,int id=0);
    /** Lo inicializa*/
    void init();

public slots:
    unsigned char getMode();
    void setMode(unsigned char valor);
    void setUsed(bool used);
    void setFlag(char flag, unsigned char value);
    char getFlag(char flag);
    void setAutoStart(bool value);
    void setRefTemperature(double valor);
    //void sltModbusDataSent(QByteArray data);
    //void sltModbusDataReceived(QByteArray data);

private:
    int myID;
    quint16 statusFlags;
    quint16 alarm1Flags;
    quint16 alarm2Flags;
    quint16 alarm3Flags;
    quint16 slaveAddress;
    int m_ID;
    double dTemperatura;
    double dPresion;
    double dRefTemperatura;

    quint16 sTemperatura;
    quint16 sPresion;
    quint16 sRefTemperatura;

    unsigned char m_mode;
    bool m_used;
    bool m_autostart;
    char m_FlagErrorIniciando;

    bool m_ok;


signals:
    //void sglNewDataSent(QByteArray data);
    //void sglNewDataReceived(QByteArray data);

    void sglNewDataToSend(QByteArray data);
    
public slots:
    void readAllData(quint16 address);
    void readSetPoint(quint16 address);
    void readTemperature(quint16 address);
    void readStatus(quint16 address);
    void readSetpoint(quint16 address);
    void writeSetpoint(quint16 address);
    double convertToTemperature(quint16 registro);
    double convertToPressure(quint16 registro);
    double convertToResistivity(quint16 registro);
    double convertToSetTemperature(quint16 registro);
    double convertSetTemperature2double(quint16 registro);
    double convertTemperature2double(quint16 registro);
    double convertPressure2double(quint16 registro);
    void convertTemperature();
    void convertSetTemperature();
    void convertPressure();
    quint16 convertToSetTemperatureRegister(double valor);
    void runThisChiller(quint16 address);
    void stopThisChiller(quint16 address);
};

#endif // IN2SMCCONTROL_H
