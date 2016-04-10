#ifndef In2WenglorControl_H
#define In2WenglorControl_H

#include <QObject>
#include "gestorserie.h"


class In2WenglorControl : public QObject
{
    Q_OBJECT
public:
    explicit In2WenglorControl(QObject *parent = 0,int id=0);
    /** Lo inicializa*/
    void init();

public slots:
    unsigned char getMode();
    void setMode(unsigned char valor);
    //void sltModbusDataSent(QByteArray data);
    //void sltModbusDataReceived(QByteArray data);
    //double convertToDiameter(quint16 registro);
    //double convertDiameter2double(quint16 registro);
    //void convertDiameter();

private:
    int myID;
    quint16 statusFlags;
    quint16 alarm1Flags;
    quint16 alarm2Flags;
    quint16 alarm3Flags;
    int m_ID;


    unsigned char m_mode;
    bool m_used;
    bool m_autostart;
    char m_FlagErrorIniciando;

    bool m_ok;


signals:



};

#endif // In2MaxSonarControl_H
