#ifndef IN2CONFIGURATORPRINTER_H
#define IN2CONFIGURATORPRINTER_H

#include <QObject>

class In2ConfiguratorPrinter : public QObject
{
    Q_OBJECT
public:
    explicit In2ConfiguratorPrinter(QObject *parent = 0);
    ~In2ConfiguratorPrinter();
    void readConfig();
    bool fileExists();
    QString configFile();

signals:

public slots:
};

#endif // IN2CONFIGURATORPRINTER_H
