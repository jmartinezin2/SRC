#ifndef IN2GESTORFLUIDICA_H
#define IN2GESTORFLUIDICA_H

#include <QObject>

#include "in2hydracontrol.h"
#include "globals.h"
#include <QVector>
#include "MemoryMap.h"

#define FLAG_ESTADO_FLUIDICA 1
#define FLAG_FLUIDICA_OK    2
#define FLAG_COMFLUID_OK    3


#define FLUID_TIMEOUT_MONITORING 2000

#define COUNTER 5

//Estados a informar
enum FLUSTATES{
    FLU_OK=0,
    FLUINIT_ERROR=1,
    FLU_COMERROR=2
};

enum HYDRA_ERROR{
    HYDRAERROR_MENISCUS_RANGE=0,
    HYDRAERROR_MENISCUS_POSITIVE,
    HYDRAERROR_BITS_ERROR,
    HYDRAERROR_FAULTS_ERROR,
    HYDRAERROR_SWITCHES_ERROR,
    HYDRAERROR_STATECHANGE_ERROR,
    HYDRAERROR_TEMPERATURE_ERROR,
    HYDRAERROR_LOW_LEVEL
};

struct strFiltroNivel{
    short level[COUNTER];
    int index;
};


class In2GestorFluidica : public QObject
{
    Q_OBJECT

public :
    static In2GestorFluidica *request(QObject *parent);
    void Init();
    void SetFlag(char flag,char value);
    char GetFlag(char flag);
    void InitEquipo(int x);
    void StopEquipo(int x);
    void ShutdownEquipo(int x);

    void RebootEquipo(int x);
    int numEquipos();
    XCL_ADC getSupplyTemp(int x);
    XCL_ADC getLevelRes(int x);
    XCL_ADC getReturnPre(int x);
    XCL_ADC getSupplyPre(int x);
    XCL_ADC getReturnTemp(int x);
    XCL_ADC getResTemp(int x);
    short getSwitches(int x);
    short getBits(int x);
    short getFaults(int x);
    float*  getCoeffF(int x);
    short*  getCoeffI(int x);
    float getSupplyOffset(int index);
    float getReturnOffset(int index);
    QString getDescription(int index);
    QString getModeDescription(int index);
    QString getFaultsDescription(int x);
    QString getSwitchesDescription(int x);
    QString getBitsDescription(int x);
    XCL_Modes getMode(int index);
    QColor getColor(int index);
    float getMeniscusPresure(int index);
    float getDiferentialPresure(int index);
    void sltTestAllDevicesState();
    bool deviceReadyToPrint(int index);



private:
    In2GestorFluidica(QObject *parent = 0);

    int readPIDPresure(int c);
    void CalculateMeniscusSetPoint(int x);
    void filterMeniscusRange(int x);
    void filterMeniscusPositive(int x);
    void filterNivel(int x);
    void filterBits(int x);
    void filterFaults(int x);
    void filterSwitches(int x);
    void filterCambioEstado(int x);
    void filterTemperatura(int x);

private slots:
    void sltGetFluidicData();
    void sltWaitForCompleteShutdown();
    void sltSetNumEquipos(int val);
    void sltError(int val);
public slots:
    void sltMonitoring();
    void sltUpdateInfo();
    void startMonitoring();
    void stopMonitoring();
    DWORD sltGetCurrentThread();
    void sltAutoShutdown();
    void CompleteShutdown();



private:
    static In2GestorFluidica *mInstance;
    QVector<In2HydraControl*> m_equipos;    
    QString m_Descripcion;
    char m_FlagEstado;
    char m_FlagFluidOK;
    char m_FlagComunicacionFluidOK;
    char m_FlagInitError;
    XCL_ADC ADC_Res_Temp[MAXHYDRAS];
    XCL_ADC ADC_Supply_Temp[MAXHYDRAS];
    XCL_ADC ADC_Return_Temp[MAXHYDRAS];
    XCL_ADC ADC_Supply_Pr[MAXHYDRAS];
    XCL_ADC ADC_Return_Pr[MAXHYDRAS];
    XCL_ADC ADC_Level_Res[MAXHYDRAS];
    XCL_Errors error[MAXHYDRAS];
    XCL_Modes m_Modes[MAXHYDRAS];
    short switches[MAXHYDRAS];
    short bits[MAXHYDRAS];
    short faults[MAXHYDRAS];
    struct strFiltroNivel m_FiltroNivel[MAXHYDRAS];
    float CoeficientesF[MAXHYDRAS][MSP_NUM_FC];
    short CoeficientesI[MAXHYDRAS][MSP_NUM_IC];
    float m_PreDif[MAXHYDRAS];
    float m_PreMeniscus[MAXHYDRAS];
    int m_MeniscusCounter[MAXHYDRAS];
    char m_MeniscusRangeFault[MAXHYDRAS];
    char m_MeniscusFault[MAXHYDRAS];
    short m_numEquipos;
    XCL_PID XCLPidPresure[MAXHYDRAS];
    float m_MeniscusSetPoint[MAXHYDRAS];
    float m_supplyOffset[MAXHYDRAS];
    float m_returnOffset[MAXHYDRAS];
    bool bMonitoringEnabled;




signals:
    void FluidStateNewValue(char value);
    void SendDebugInfo(unsigned char level,const QString &txt);
    void HydraError(int error, int equipo,float value);
    void CompleteShutdownFinished();
    void sgnChangeState(int theme);

public slots:
    
};

#endif // IN2GESTORFLUIDICA_H
