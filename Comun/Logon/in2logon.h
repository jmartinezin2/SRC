#ifndef IN2LOGON_H
#define IN2LOGON_H

#include <QObject>
#include "logger.h"
#include "globals.h"
#include "MemoryMap.h"


//Grupo de root de In2
#define IN2ROOTGROUP "in2root"
//Grupo de printers
#define IN2PRINTERGROUP "in2printer"


struct LogonConfig{
    QString m_host;
    QString m_Dn;
    QString m_filterRoot;
    QString m_filterPrinter;
    QString m_LocalHost;
};

class In2Logon : public QObject
{
    Q_OBJECT

public:
    In2Logon(QObject *parent = 0);
    int CheckDA();
    int CheckDA(const QString & group);
    int CheckLocal();
    int CheckDARoot();
    int CheckDAPrinter();
    int TestLocalUser();

signals:
    void SendDebugInfo(unsigned char code, const QString & msg);

private:
    struct LogonConfig m_Config;
    MemoryMap *m_memoryMap;

public slots:
    
};

#endif // IN2LOGON_H
