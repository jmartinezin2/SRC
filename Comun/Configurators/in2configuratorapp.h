#ifndef IN2CONFIGURATORAPP_H
#define IN2CONFIGURATORAPP_H

#include <QMainWindow>
#include "ui_in2configappui.h"

class In2ConfiguratorApp : public QMainWindow
{
    Q_OBJECT
public:
    explicit In2ConfiguratorApp(QWidget *parent = 0);
    ~In2ConfiguratorApp();
    void setGridEnabled(bool val);

signals:
    void sgnEnableGrid(bool enable);

private slots:
    void sltLoadConfigApp();
    void sltLoadConfigDesigner();
    void sltLoadDefaultConfig();
    void showMsg(QString txt);
    void sltEnableGrid(bool enable);

private:
    Ui::In2ConfigAppUI *ui;
public slots:
};

#endif // IN2CONFIGURATORAPP_H
