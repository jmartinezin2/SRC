#ifndef IN2CONFIGXUSB_H
#define IN2CONFIGXUSB_H

#include <QDialog>
#include "printparameters.h"

namespace Ui {
class In2ConfigXUSB;
}

class In2ConfigXUSB : public QDialog
{
    Q_OBJECT
    
public:
    explicit In2ConfigXUSB(QWidget *parent = 0,QString strConfigFile="");
    ~In2ConfigXUSB();

private slots:
    void sltSaveData();
    void sltTestHeads();
    void sltEnableAllRow1(bool enable);
    void sltEnableAllRow2(bool enable);
    void sltUpdateGOffsetmm();
    void sltUpdateGOffsetpuntos();
    
private:
    Ui::In2ConfigXUSB *ui;
    CPrintParameters *m_parameters;
    int m_card;
    QString m_strConfigFile;
};

#endif // IN2CONFIGXUSB_H
