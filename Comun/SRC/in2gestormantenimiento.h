#ifndef IN2GESTORMANTENIMIENTO_H
#define IN2GESTORMANTENIMIENTO_H

#include <QObject>
#include "In2GestorDB.h"

class In2GestorMantenimiento : public QObject
{
    Q_OBJECT
public:
    explicit In2GestorMantenimiento(QObject *parent = 0);
    long checkSegundosLampara();
    void updateSegundosLampara(long sg);
    QString checkCambioFiltros();
    bool getCambioFiltros();
    QStringList getAllRegisters();
    bool insertRegister(QString Equipo,QString Fecha,QString tiempo, QString user );

signals:
    
public slots:

signals:
    //void sgnMantenimiento(int sign);

private:
    long m_segundosLampara;
    QString m_qstrFechaCambioFiltro;
    bool m_bCambioFiltros;
    bool m_bCambioLampara;
};

#endif // IN2GESTORMANTENIMIENTO_H
