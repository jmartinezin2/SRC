#ifndef IN2MANAGERREPORT_H
#define IN2MANAGERREPORT_H

#include <QObject>
#include "in2reportgenerator.h"
#include "in2Estevereportgenerator.h"
#ifndef PHARMA
    #include "in2cinfapillsreportgenerator.h"
#endif

class In2ManagerReport : public QObject
{
    Q_OBJECT
public:
    explicit In2ManagerReport(QObject *parent = 0);
    void setTrabajo(struct In2Trabajo & t);
    ~In2ManagerReport();
    bool getAuto();


signals:
    void SendDebugInfo(unsigned char code, QString msg);
    void In2SignedReportFinished();
    void In2ReportFinished();
    void In2NewReportGenerated(QString name);
    void In2ReportFailed(const QString & txt);
    void In2ReportDataBaseData(QString lst);
public slots:
    void sltGenerateReport();
    void sltSetOutputName(QString name);
private:
    bool SignFile();
    void _setEncabezado(QPainter *painter,QString txt);

private:
    QString m_signCommand;
    QString m_signFile;
    QString m_signPassword;
    QString m_Logo;
    bool m_signActive;
    bool m_auto;
    QString m_outputPath;
    QString m_BBDD;
    QString m_wselect;
    QString m_name;
    QString m_client;
    int m_width;
    struct In2Trabajo m_Trabajo;
    In2ReportGenerator *m_In2Gen;
    In2EsteveReportGenerator *m_In2EsteveGen;
#ifndef PHARMA
    In2CinfaPillsReportGenerator *m_In2CinfaPillsGen;
#endif
};

#endif // IN2MANAGERREPORT_H
