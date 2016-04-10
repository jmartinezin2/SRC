#ifndef IN2HISTORICO_H
#define IN2HISTORICO_H

#include <QWidget>
#include "ui_historicoUI.h"



class In2Historico : public QWidget
{
    Q_OBJECT
public:
    explicit In2Historico(QWidget *parent = 0);

private slots:
    void sltExit();
    void sltExportToCVS();
    void sltShowInfo(int x,int y);
    void sltGenerateReport();
    void sltReportGenerated();

private:
    //Pantalla
    Ui::HistoricoUI *ui;
    int m_numTrabajos;

signals:
    
public slots:
    
};

#endif // IN2HISTORICO_H
