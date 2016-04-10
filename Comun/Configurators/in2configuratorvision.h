#ifndef IN2CONFIGURATORVISION_H
#define IN2CONFIGURATORVISION_H

#include <QObject>
#include <QVector>
#include "in2filtrovision.h"

class In2ConfiguratorVision : public QObject
{
    Q_OBJECT
public:
    explicit In2ConfiguratorVision(QObject *parent = 0);
    void readConfig();
    void saveConfig(QVector<In2FiltroVision> filters);
    bool isEnabled();
    int expectedFilters();
    QVector<In2FiltroVision> getFiltros();
    QString getErrorPath();
    QString getGoodPath();
    QString getIP();
    QString getPort();
    QString getVendor();
    QString getMAC();
    float getCX();
    float getCY();
    
signals:

private:
    bool m_bEnabled;
    QVector<In2FiltroVision> m_filtros;
    int m_expectedFilters;
    float m_CX;
    float m_CY;
    QString m_ErrorImagePath;
    QString m_GoodImagePath;
    QString m_IP;
    QString m_MAC;
    QString m_port;
    QString m_vendor;
public slots:
    
};

#endif // IN2CONFIGURATORVISION_H
