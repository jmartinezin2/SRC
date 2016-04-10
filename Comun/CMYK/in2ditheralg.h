#ifndef IN2DITHERALG_H
#define IN2DITHERALG_H

#include <QWidget>
#include "ui_in2ditheralgui.h"
#include "MemoryMap.h"

class In2DitherAlg : public QWidget
{
    Q_OBJECT
public:
    In2DitherAlg(QWidget *parent,struct m_Colores & color);


private slots:
    void sltSaveExit();
    void sltCancelExit();
    void sltReload();
    void sltUpdatePalette();


signals:
    void sgnReload(struct m_Colores & color);


private:
    Ui::In2DitherAlg *ui;
    struct m_Colores m_color;


public slots:
    void sltReloadPalette();
};

#endif // IN2DITHERALG_H
