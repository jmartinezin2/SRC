#ifndef In2GestorDiametro_H
#define In2GestorDiametro_H

#include <QObject>
#include "in2wenglorcontrol.h"
#include "gestorserie.h"

#include <QVector>
#include <QByteArray>

#define FLAG_ESTADO_IN2GD     1
#define FLAG_IN2GD_OK         2
#define FLAG_COMIN2GD_OK      3
#define MAXSONARS             4

#define TIMEOUT_MONITORING 5000

//Estados a informar
enum IN2GDSTATES{
    IN2GD_OK=0,
    IN2GD_INIT_ERROR=1,
    IN2GD_COMERROR=2
};


class In2GestorDiametro : public QObject
{
    Q_OBJECT

public :
    //static In2GestorDiametro *request(QObject *parent,int equipos);
    In2GestorDiametro(QObject *parent = 0,int equipos=0);
    GestorSerie *MiGestorSerie;

    void Init();
    void SetFlag(char flag,char value);
    char GetFlag(char value);
    void setSetpoint(int x,quint16 value);
    void setStatus(int x,quint16 value);
    void setAlarm1(int x,quint16 value);
    void setAlarm2(int x,quint16 value);
    void setAlarm3(int x,quint16 value);
    void setReady(int x,bool value);

    int numEquipos();

    void setCurrentDiameter(int index, double valor);
    void setRawDiameter(QString txt);
    QString getRawDiameter();
    double getCurrentDiameter(int index);

    void setMaxDiameter(int index,double valor);
    double getMaxDiameter(int index);

    void setMinDiameter(int index,double valor);
    double getMinDiameter(int index);

    void setFilteredDiameter(int index,double valor);
    double getFilteredDiameter(int index);

    bool getReady(int x);

    bool getCurrentChange(int x);
    bool getMaxChange(int x);
    bool getMinChange(int x);
    bool getFilteredChange(int x);
    void setCurrentChange(int x,bool valor);
    void setMaxChange(int x,bool valor);
    void setMinChange(int x,bool valor);
    void setFilteredChange(int x,bool valor);


private:

    //static In2GestorDiametro *mInstance;
    QVector<In2WenglorControl*> m_equipos;
    QVector<double> diametroActual;
    int nElementosBuffer;
    int indiceVector;
    short m_numEquipos;
    QString m_Descripcion;
    bool estadoPuerto;
    double diameterCurrent_d[MAXSONARS];
    double diameterMax_d[MAXSONARS];
    double diameterMin_d[MAXSONARS];
    double diameterFilter_d[MAXSONARS];
    QString m_rawDiameter;
    //quint16 diameterCurrent[MAXSONARS];
    //quint16 diameterMax[MAXSONARS];
    //quint16 diameterMin[MAXSONARS];
    //quint16 diameterFilter[MAXSONARS];

    bool diameterCurrentChange[MAXSONARS];
    bool diameterMaxChange[MAXSONARS];
    bool diameterMinChange[MAXSONARS];
    bool diameterFilterChange[MAXSONARS];

    bool readyToPrint[MAXSONARS];
    bool readyChange[MAXSONARS];

    char m_FlagEstado;
    char m_FlagIn2GDOK;
    char m_FlagComunicacionIn2GDOK;

    double calibracion_db1;
    double calibracion_s1;
    double calibracion_db2;
    double calibracion_s2;


public slots:
    void filterCurrent(int x);
    void filterMax(int x);
    void filterMin(int x);
    void filterFilter(int x);

    void sltGetDiameterData();
    void sltOpenPort(QString port);
    void sltClosePort();

    void sltReadWenglorDiameter();

    void sltMonitoring();
    void sltDataReceived(QByteArray datos);
    void sltResetStatistics(int equipo);

    void sltDatosCalibracion(double x1, double y1, double x2, double y2);

private slots:
    double convert2Diameter(quint16 valorLeido);

signals:
    void In2GDStateNewValue(char value);
    void SendDebugInfo(unsigned char level,const QString &txt);
    void In2GDError(const QString &txt);
    void sglCambioActual(int equipo);
    void sglCambioMaximo(int equipo);
    void sglCambioMinimo(int equipo);
    void sglCambioFiltrado(int equipo);

    void sglNewReceivedMessage(QByteArray data);
    void sglNewSentMessage(QByteArray data);
    void sglEstadoPuerto(bool estado);

    void sglDiameterSendData();


};

#endif // In2GestorDiametro_H
