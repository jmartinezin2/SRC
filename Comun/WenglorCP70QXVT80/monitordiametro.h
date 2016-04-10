/** @addtogroup InkjetForm
 *  @{*/
/** @file ClienteSMTP.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo principal para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ClienteSMTP.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#ifndef MonitorDiametro_H
#define MonitorDiametro_H

#define DEFAULT_LOG_FILENAME    "diameterLaserLog"

#include <QObject>
#include "in2gestordiametro.h"
//#include "clienteopc.h"

class MonitorDiametro : public QObject
{
    Q_OBJECT

public:
    //static MonitorDiametro *request(QObject *parent);
    explicit MonitorDiametro(QObject *parent=0);
    ~MonitorDiametro();
    bool isEnabled();
    bool isPLCEnabled();
    QString getRawDiameter();
    double getCurrentDiameter(int index);
    void setIniFile(QString file);
    void storeValueFromPLC(int value);



public slots:

    void init();
    void sltActualizaDatos_1();

private:
    //static MonitorDiametro * mInstance;

    In2GestorDiametro *miGestorDiametro;
    bool primeravez;
    bool showInputMessages;
    bool showLog;
    QString puerto;
    int iPuerto;
    bool isPortOpen;
    int numeroEquipos;
    int contadorMonitor;
    int contadorSaveToFile;
    QByteArray contentsToSave;
    unsigned char m_pendingToSave;
    QString m_iniFile;
    bool monitorEnabled;
    bool PLCEnabled;

    double cal_db1;
    double cal_s1;
    double cal_db2;
    double cal_s2;
    double m_valueFromPLC;
    int m_rawValueFromPLC;


signals:
    void sglOpenPort(QString port);
    void sglClosePort();
    void sglResetStatistics(int equipo);
    void sglReadWenglorDiameter();
    void sglDatosCalibracion(double x1,double y1,double x2,double y2);
    void sgnWenglorDiameter(double data);

private slots:
    void sltError(const QString &txt);
    void sltCambioActual(int equipo);
    void sltCambioMaximo(int equipo);
    void sltCambioMinimo(int equipo);
    void sltCambioFiltrado(int equipo);
    void sltCambioReady(int equipo);
    void openPort();
    void closePort();
    void abrirFicheroIni();
    void sltEstadoPuerto(bool estado);

};

#endif // MonitorDiametro_H
