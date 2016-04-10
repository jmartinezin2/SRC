#ifndef IN2MACHINEINFO_H
#define IN2MACHINEINFO_H

#include <QWidget>

namespace Ui {
class In2MachineInfo;
}

class In2MachineInfo : public QWidget
{
    Q_OBJECT
    
public:
    explicit In2MachineInfo(QWidget *parent = 0);
    ~In2MachineInfo();

protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::In2MachineInfo *ui;
};

#endif // IN2MACHINEINFO_H
