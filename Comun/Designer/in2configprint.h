#ifndef IN2CONFIGPRINT_H
#define IN2CONFIGPRINT_H

#include <QWidget>
#include "ui_In2ConfigPrint.h"
#include "MemoryMap.h"

class In2ConfigPrint : public QWidget
{
    Q_OBJECT
public:
    explicit In2ConfigPrint(QWidget *parent = 0);
    
signals:
    void SendDebugInfo(unsigned char val, QString txt);
private slots:
    void sltSaveAndExit();
    void sltCheckConfig();
    void sltUpdateOffsetMm();
    void sltUpdateOffsetPoints();


public slots:

private:
    Ui::In2ConfigPrintUI *ui;
    MemoryMap *m_memoryMap;
    int m_Maculas;
    
};

#endif // IN2CONFIGPRINT_H
