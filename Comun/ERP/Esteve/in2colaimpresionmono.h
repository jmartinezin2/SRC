#ifndef IN2COLAIMPRESIONMONO_H
#define IN2COLAIMPRESIONMONO_H

#include <QMainWindow>
#include "in2graphicsscene.h"
#include "in2graphicsview.h"
#include "ui_In2ColaImpresionMono.h"

class In2ColaImpresionMono : public QMainWindow
{
    Q_OBJECT
public:
    explicit In2ColaImpresionMono(QWidget *parent = 0);


private slots:
     void sltDataFromERP(const QStringList & p);
     void sltDataFromERPOut(const QStringList & p);
     void sltDataFromPendingOrder(const QStringList & p);
     void sltCheckERP();
     void sltCheckERPOut();
     void sltShowERPData();
     void sltShowImage();
     void sltLoad();
     void sltExit();
     void sltWorkLoaded(int mode);
     void sltCheckAndLoad();
     void _sltCheckAndLoad();
     void sltInfoMessage(QString & txt);
     void sltClearInfoMessage();
     void sltFocusScan();
     void setCheckERP(bool val);
     void _loadPendingOrder();




signals:
     void SendDebugInfo(unsigned char code, QString msg);
     void sgnWorkLoaded(bool val);
     void sgnColaNewMetrosRef(int metros);
    
private:
    Ui::In2ColaImpresionMono *ui;
    In2GraphicsScene *m_scene;
    In2GraphicsView *m_view;
    QWidget *m_parent;
    bool m_workLoaded;
    QString m_actualOT;
    bool m_pendingScan;
    bool m_bCheckERP;
    bool m_pedingOrder;

public slots:
    
};

#endif // IN2COLAIMPRESIONMONO_H
