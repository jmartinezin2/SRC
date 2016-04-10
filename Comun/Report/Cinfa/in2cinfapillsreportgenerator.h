#ifndef IN2CINFAPILLSREPORTGENERATOR_H
#define IN2CINFAPILLSREPORTGENERATOR_H

#include <QObject>
#include "MemoryMap.h"
#include "in2defines.h"

class In2CinfaPillsReportGenerator : public QObject
{
    Q_OBJECT
public:
    explicit In2CinfaPillsReportGenerator(QObject *parent = 0);
    void setTrabajo(struct In2Trabajo & t);
    ~In2CinfaPillsReportGenerator();
    bool getAuto();
    bool SignFile();
    void _setEncabezado(QPainter *painter,QString txt);


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
    int m_width;
    struct In2Trabajo m_Trabajo;
    MemoryMap *m_memoryMap;

};

#endif // IN2CINFAPILLSREPORTGENERATOR_H
