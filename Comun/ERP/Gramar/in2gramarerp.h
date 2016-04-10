#ifndef IN2GRAMARERP_H
#define IN2GRAMARERP_H

#include <QObject>
#include <QStringList>

class In2GramarErp : public QObject
{
    Q_OBJECT
public:
    In2GramarErp(QObject *parent = 0);
    QStringList checkERP();
    
signals:
    void SendDebugInfo(unsigned char val,QString txt);


private:


private:


public slots:
    
};

#endif // IN2GRAMARERP_H
