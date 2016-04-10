#ifndef IN2GESTORMAQUINA_H
#define IN2GESTORMAQUINA_H

#include <QObject>
#include "gestormaquina.h"
#include "monitordiametro.h"

class In2GestorMaquina : public QObject
{
    Q_OBJECT
public:
    static In2GestorMaquina *request(QObject *parent);
    void InitGestor();
    void InitEmpalme();
    void SoftStop();
    void EmergencyStop(int mode);
    QString getCodeErrorMaquina();
    bool isPrinting();
    double _getMaculaMax();
    double _getMaculaMin();
    bool startDelayed();

private:
    In2GestorMaquina(QObject *parent = 0);

    
signals:
    void SendDebugInfo(unsigned char level,QString data);
    void sgnShowLog();
    void sgnChangeState(int state);
    void sgnIn2GestorMaquinaFinImpresion();
    void In2NewReportGenerated(QString name);
    void sgnIn2GestorMaquinaMessage(const QString & txt);
    void sgnUpdateReferences();
    void sgnGesMaquinaFinEmpalmeEntrada();
    void sgnGesMaquinaFinEmpalmeSalida();
    void sgnGestorMaquinaFinImpresion();
    void sgnInitImpresion(bool);
    void sgnFinTrabajo();
    void sgnFinImpresion(int mode);
    void sgnFinMacula(int type);

public slots:
    void sltEnableMonitorPrint(bool val);
    bool ComprobacionesPreviasImpresion();
    void sltChangeMetrosReferencia(int metros);
    void sltTrabajoNuevo();
    void sltVisionMessage(QString txt);
    void sltCheckMaxBobina();


private slots:
    void sltSoftStopPerformed();
    void sltFlancoOPC(int ID);
    void _sltOPCSecuenciaOn();
    void _sltOPCCodigoParada();
    void _sltMonitorUsoLampara();
    void _sltMonitorTemperaturaLampara();
    void _sltMonitorMetrosImpresos();
    void _sltMonitorDiametroEntrada();
    void _sltMonitorElectronica();
    void _sltMonitorDistanciaMaculas();
    void sltLaunchPrint();
    void sltMonitorPrintLoop();
    void sltChangeTheme(int theme);
    int getTheme();
    void sltFinalizarImpresion(int mode);
    void sltEnableMonitoring();
    void sltReportFinImpresion();
    void sltMonitorDelayed();



public slots:

private:
    static In2GestorMaquina *mInstance;
    GestorMaquina *m_gestorMaquina;
    bool m_MonitorPrinting;
    MonitorDiametro *m_MonDiametro;
    bool m_Printing;
    int m_PrevTheme;
    QElapsedTimer m_elapsedTimer;
    MemoryMap *m_memoryMap;
    QString m_extMessage;
    bool m_startDelayed;
    int m_maxMaculaCounter;
    int m_minMaculaCounter;

    
};

#endif // IN2GESTORMAQUINA_H
