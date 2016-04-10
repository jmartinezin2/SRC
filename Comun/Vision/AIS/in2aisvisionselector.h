#ifndef IN2AISVISIONSELECTOR_H
#define IN2AISVISIONSELECTOR_H

#include <QObject>
#include <QMainWindow>
#include "ui_in2aisvisionselection.h"
#include "in2graphicspixmapitem.h"
#include "in2graphicsscene.h"
#include "in2graphicsscene.h"
#include "in2graphicsview.h"
#include "in2aisvisionobject.h"
#include "in2gestorais.h"

class In2AISVisionSelector : public QMainWindow
{
    Q_OBJECT
public:
    explicit In2AISVisionSelector(QWidget *parent = 0);
    ~In2AISVisionSelector();
    void sltLoadIniFile(QString file);
    void loadBMP(QString file);
    void loadProgram(QString file);
    
private slots:
    void sltLoadImage();
    void sltExit();
    void sltAddItem();
    void sltDeleteItem();
    void sltAddItem(In2AISVisionObject obj);
    void sltUpdateGeometry(int id, QRect & rect);
    void sltLoadFile();
    void sltSend();
    void sltConnected();
    void sltPing();
    void sltWOL();
    void sltApagar();
    void sltReiniciar();
    void sltDisConnected();
    void sltPublishInfo(QString txt);
    void sltReceivedDatagram(QString txt);
    void sltGetConfigFromAIS(QString txt);
    void sltRegenConfigString();
    void sltOCRChange(int x);
    QVector<In2AISVisionObject> getData();
    void sltVisionLoad();
    void sltVisionSave();
    void sltVisionConfig();
    void sltCellClicked(int x, int y);
    void UpdateMousePos(const QPointF &p );
    QColor getItemColor(int index);
    void sltRemapear();
    void sltAplicacionConnected();
    void sltAplicacionDisConnected();
    void sltEquipoConnected();
    void sltEquipoDisConnected();
    void sltConectar();
    void sltMessageFromVision(unsigned char value, QString txt);



signals:
    
private:
    Ui::In2AISVisionSelection *ui;
    In2GraphicsScene *m_scene;
    In2GraphicsView *m_view;
    QWidget *m_parent;
    QString m_file;    
    int m_counter;
    float m_offsetX;
    float m_offsetY;

public slots:
    
};

#endif // IN2AISVISIONSELECTOR_H
