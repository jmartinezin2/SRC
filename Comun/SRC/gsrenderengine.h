#ifndef GSRENDERENGINE_H
#define GSRENDERENGINE_H

#include <QObject>

class GsRenderEngine : public QObject
{
    Q_OBJECT
public:
    explicit GsRenderEngine(QObject *parent = 0);
    QString getConfigFile();
    void saveConfigFile();
    int executeCommand(const QString &d);
    int render();
    void setInputFile(const QString & d);
    void setOutputFile(const QString &d);
    void setDevice(const QString &d);
    void setResolution(const QString &d);
    void setDoubleResolution();
    void setDitheringBits(int value);
    QString getResolution();
    void setICCFile(const QString &d);
    QString getICCFile();
    QString getLastCommand();
    QString getExecutable();
    void setExecutable(const QString & d);
    QString getDevice();
    void setExtraOptions(const QString &d);
    QString getExtraOptions();
    void install();
    bool testGsInstallation();


private:


private:
    QString m_strExecutable;
    QString m_strInputFile;
    QString m_strOutputFile;
    QString m_strDevice;
    QString m_strResolution;
    QString m_strExtraOptions;
    QString m_strICCFile;
    QString m_command;
    QString m_strInstaller;
    bool m_doubleRes;


signals:

    void SendDebugInfo(unsigned char level, const QString &d );

public slots:
    
};

#endif // GSRENDERENGINE_H
