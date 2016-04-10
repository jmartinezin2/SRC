#ifndef IN2PRINTERCONFIGURATOR_H
#define IN2PRINTERCONFIGURATOR_H

#include <QObject>

class In2PrinterConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit In2PrinterConfigurator(QObject *parent = 0);
    ~In2PrinterConfigurator();

signals:

public slots:
};

#endif // IN2PRINTERCONFIGURATOR_H
