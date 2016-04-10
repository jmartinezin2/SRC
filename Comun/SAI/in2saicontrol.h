#ifndef IN2SAICONTROL_H
#define IN2SAICONTROL_H

#include <QObject>
#include <QHostInfo>
class In2SaiControl : public QObject
{
    Q_OBJECT
public:
    explicit In2SaiControl(QObject *parent = 0);
    bool isEnabled();

signals:
    void SendDebugInfo(unsigned char code, QString msg);
    void sgnSAIActive();
    void sgnError(int val);
private:
    bool testCondicionFichero();
    bool testCondicionRed();
    void shutdown();

public slots:
    void sltStartMonitor();
    void sltStopMonitor();
private slots:
    void sltMonitor();
    void lookedUp(const QHostInfo & host);
private:
    bool m_bEnabled;
    QString m_strFile;
    bool m_bMonitor;
    int m_timer;

public slots:
    
};

#endif // IN2SAICONTROL_H
