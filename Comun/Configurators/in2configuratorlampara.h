#ifndef IN2CONFIGURATORLAMPARA_H
#define IN2CONFIGURATORLAMPARA_H

#include <QString>

//Tipos de lamparas
#define PHOSEON 0
#define HONLE 1

#define LAMPARA_DB "Database\\In2Lampara.sqlite"


class In2ConfiguratorLampara
{
public:
    In2ConfiguratorLampara();
    int getVendor();
    QString getStrVendor();
    bool getPosDinamico();
    bool getIntDinamico();
    bool getEnabled();

private:
    QString m_strVendor;
    int m_Vendor;
    bool m_IntDinamico;
    bool m_PosDinamico;
    bool m_enabled;

};

#endif // IN2CONFIGURATORLAMPARA_H
