#ifndef IN2REPORTCONFIGURATOR_H
#define IN2REPORTCONFIGURATOR_H

#include <QObject>

class In2ReportConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit In2ReportConfigurator(QObject *parent = 0);
    void readConfig();
    void setSignCommand(QString txt);
    void setSignFile(QString txt);
    void setSignPassword(QString txt);
    void setLogo(QString txt);
    void setSignActive(bool val);
    void setOutputPath(QString txt);
    void setBBDD(QString txt);
    void setWselect(QString txt);

    QString getSignCommand();
    QString getSignFile();
    QString getSignPassword();
    QString getLogo();
    bool getSignActive();
    bool getAuto();
    QString getOutputPath();
    QString getBBDD();
    QString getWselect();



signals:

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
    
public slots:
    
};

#endif // IN2REPORTCONFIGURATOR_H
