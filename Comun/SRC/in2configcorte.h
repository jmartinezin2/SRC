#ifndef IN2CONFIGCORTE_H
#define IN2CONFIGCORTE_H

#include <QWidget>
#include "ui_in2configcorte.h"

class In2ConfigCorte : public QWidget
{
    Q_OBJECT
public:
    explicit In2ConfigCorte(QWidget *parent = 0);

private slots:
    void sltExit();
    void sltChangeFuerza();
    void sltActivate();
    void sltUpdateData();

signals:
    void sgnNewDataLampara(int item, QVariant value);

private:
    Ui::In2ConfigCorteUI *ui;

public slots:
};

#endif // IN2CONFIGCORTE_H
