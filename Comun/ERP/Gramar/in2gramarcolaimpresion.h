#ifndef IN2GRAMARCOLAIMPRESION_H
#define IN2GRAMARCOLAIMPRESION_H

#include <QMainWindow>
#include "ui_In2GramarColaImpresion.h"
class In2GramarColaImpresion : public QMainWindow
{
    Q_OBJECT
public:
    explicit In2GramarColaImpresion(QWidget *parent = 0);

private slots:
    void sltExit();
    void sltCheckERP();
    void sltCheckIn2ERP();
    void sltSelectWork(int x, int y);
    void sltSelectGramarWork(int x, int y);
    void sltInsertWork();
    void sltLoadWork();
    bool sltGenerateFromIn2();
    void sltPrevFromIn2();
    void sltPrevFromGramar();
    void sltDeleteItem();
    void sltShowMessage(unsigned char value,QString txt);

signals:
    void SendDebugInfo(unsigned char calue, QString txt);
    void sgnWorkLoaded(bool val);
    void sgnERPNewMetrosRef(int metros);
    void sgnERPTrabajoNuevo();

public slots:

private:
    int iprevGrammarItem;
    int iprevIn2Item;
    QStringList m_lstHeads;
    Ui::In2GramarColaImpresion *ui;
    bool m_load;

    
};

#endif // IN2GRAMARCOLAIMPRESION_H
