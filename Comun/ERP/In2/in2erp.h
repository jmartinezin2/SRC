#ifndef IN2ERP_H
#define IN2ERP_H

#include <QObject>
#include "in2gestorerp.h"
#include <QStringList>
#include "in2defines.h"

class In2ERP : public QObject
{
    Q_OBJECT
public:
    static In2ERP *request(QObject *parent);
    QStringList checkERP();
    bool insertCurrentWork();
    QStringList execSQL(QString txt, bool save);

signals:
    void SendDebugInfo(unsigned char x,QString txt);
    void sgnERPNewMetrosRef(int metros);

private:
    In2ERP(QObject *parent = 0);

private:
    static In2ERP *mInstance;
    In2GestorERP *m_gestorErp;
    
signals:
    
public slots:
    
};

#endif // IN2ERP_H
