#ifndef IN2COLAIMPRESION_H
#define IN2COLAIMPRESION_H

#include <QMainWindow>
#include "ui_in2colaimpresion.h"
#include "in2graphicsscene.h"
#include "in2graphicsview.h"
#include "in2graphicspixmapitem.h"

class In2ColaImpresion : public QMainWindow
{
    Q_OBJECT
public:
    explicit In2ColaImpresion(QWidget *parent = 0);
    ~In2ColaImpresion();

public slots:
    void sltExit();

signals:
    void SendDebugInfo(unsigned char val, QString txt);
    void sgnWorkLoaded();

private slots:
    void sltDataFromERP(const QStringList & data);
    void sltCheckERP();
    void sltShowImage();
    void sltLoadWork();
    void sltSelectWork(int x, int y);
    void sltSelectText(QString txt);
    void sltDeleteItem();



private:
    Ui::In2ColaImpresionUI *ui;
    QWidget *m_parent;
    In2GraphicsScene *m_scene;
    In2GraphicsView *m_view;

public slots:

};

#endif // IN2COLAIMPRESION_H
