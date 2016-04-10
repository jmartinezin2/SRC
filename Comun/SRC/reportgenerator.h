#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QObject>
#include "globals.h"
#include <QProcess>
#include <QPrinter>

//Estructura para firmar un pdf
struct st_In2RepConfig{
    QString m_signFile;
    QString m_signPassword;
    QString m_signCommand;
    QString m_inputPdfFile;
    QString m_outputPdfFile;
    QString m_outputCasualName;
    QString m_Logo;
    QString m_outputPath;
    int m_signActive;
};

struct st_In2ReportSection{
    QString title;
    QStringList arguments;
};

class ReportGenerator : public QObject
{
    Q_OBJECT
public:
    explicit ReportGenerator(QObject *parent = 0);
    void AddString(QString str);
    void AddSection(const QString & title,const QStringList & data);
    QString getReport();
    QString saveToFile();
    bool GeneratePDF();
    QString OutputPath();
    QString outputPdfFile();
    void setCasualOutputFilename(const QString &name);

private:
    void LoadConfiguration();
    void SignFile();
    void PrintSection(QPainter *paint,int section);
    int getSectionHeight(int i);
    int getHeadHeight();
    bool TestPage(int y,QPainter * paint);
    bool TestPage(int y);
    void CalculateNumPages();
    void PrintNumPage(QPainter * paint);

private:
    QString m_Filename;
    QString m_report;
    QString m_dateGeneration;
    st_In2RepConfig m_Config;
    QProcess m_signProcess;
    QVector<struct st_In2ReportSection> m_sections;
    int m_width;
    QPrinter m_printer;
    int m_pages;
    int m_totalPages;

private slots:
    void ProcessOutputError();
    void ProcessOutputOutput();
signals:
    void SendDebugInfo(unsigned char code, const QString & msg);

public slots:
    
};

#endif // REPORTGENERATOR_H
