#ifndef IN2CONFIGLAMPARA_H
#define IN2CONFIGLAMPARA_H

#include <QWidget>
#include "ui_in2configlampara.h"

class In2ConfigLampara : public QWidget
{
    Q_OBJECT
public:
    explicit In2ConfigLampara(QWidget *parent = 0);

private slots:
    void sltLoadDataIntensidad();
    void sltLoadDataPosicion();
    void sltLoadTablaIntensidad(QString table);
    void sltLoadTablaPosicion(QString table);
    void sltSaveTablaIntensidad();
    void sltSaveTablaPosicion();
    void sltAddRowIntensidad();
    void sltAddRowPos();
    void sltDeleteTableIntensidad();
    void sltDeleteTablePosicion();
    void sltJogMasOn();
    void sltJogMenosOn();
    void sltJogMasOff();
    void sltJogMenosOff();
    void sltUpdatePos();
    void sltGoto();
    void sltResetPos();
    void sltResetLampara();
    void sltExit();
    void sltGotoButton(QAbstractButton* p);
    void sltSetUpLampara();

private:
    void sltInfo(unsigned char value, QString txt);
signals:
    void SendDebugInfo(unsigned char value,QString txt);
private:
    Ui::In2ConfigLamparaUI *ui;


public slots:
};

#endif // IN2CONFIGLAMPARA_H
