#ifndef IN2CONFIGURATORPARAMS_H
#define IN2CONFIGURATORPARAMS_H

#include <QObject>

class In2ConfiguratorParams : public QObject
{
    Q_OBJECT
public:
    explicit In2ConfiguratorParams(QObject *parent = 0);
    ~In2ConfiguratorParams();
    void readConfig();

signals:

public slots:
};

#endif // IN2CONFIGURATORPARAMS_H
