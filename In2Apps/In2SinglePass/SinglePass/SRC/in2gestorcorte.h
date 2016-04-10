#ifndef IN2GESTORCORTE_H
#define IN2GESTORCORTE_H

#include <QObject>
#include "clienteopc.h"

class In2GestorCorte : public QObject
{
    Q_OBJECT
public:
    static In2GestorCorte *request(QObject *parent);
    bool isActive();
    bool sltToogleCorte();
    bool isEnabled();
    int getFuerza();
    void setFuerza(int val);

public slots:
    void sltActivate();
    void sltDeActivate();

private:
    In2GestorCorte(QObject *parent = 0);
private:
    static In2GestorCorte *mInstance;
    bool m_enabled;

signals:
    void SendDebugInfo(unsigned char val, QString txt);
    void sgnNewDataCorte(int item, QVariant value);

public slots:
};

#endif // IN2GESTORCORTE_H
