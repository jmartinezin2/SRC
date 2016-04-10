#ifndef IN2PERFILCONFIGURATOR_H
#define IN2PERFILCONFIGURATOR_H

#include <QObject>
#include "in2printperfile.h"
#include <QSettings>

class In2PerfilConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit In2PerfilConfigurator(QObject *parent = 0);
    ~In2PerfilConfigurator();
    In2PrintPerfile loadPrintPerfile(QString file);
    void savePrintPerfile(QString file,In2PrintPerfile & perfil);

signals:

private:
    QString m_file;

public slots:
};

#endif // IN2PERFILCONFIGURATOR_H
