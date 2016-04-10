#ifndef CORTEIMAGEN_H
#define CORTEIMAGEN_H

#include <QWidget>
#include <QRect>
#include "globals.h"
#include "qlabelext.h"
#include "in2graphicsscene.h"
#include "in2graphicsview.h"
#include "MemoryMap.h"


namespace Ui {
class CorteImagen;
}

class CorteImagen : public QWidget
{
    Q_OBJECT
    
public:
    explicit CorteImagen(QWidget *parent = 0);
    ~CorteImagen();

private:
    void BlockSignals(bool value);
private slots:
    void GenerateImage();
    void InitSelection(const QPoint & p);
    void ModifySelection(const QPoint &p);
    void InitSmallSelection(const QPoint & p);
    void ModifySmallSelection(const QPoint &p);
    void NewActiveZone(const QRect &r);
    void UpdateRubberFromKeypad();
    void UpdatePixelData(QString str);
    void UpdateMmData(QString str);
    void updatemmdata();
    void updatepixeldata();
    void UpdateXiniPixel(QString str);
    void UpdateXiniMm(QString str);
    void UpdateXfinPixel(QString str);
    void UpdateXfinMm(QString str);
    void UpdateYiniPixel(QString str);
    void UpdateYiniMm(QString str);
    void UpdateYfinPixel(QString str);
    void UpdateYfinMm(QString str);
    void UpdateWidthPixel(QString str);
    void UpdateWidthMm(QString str);
    void UpdateHeightPixel(QString str);
    void UpdateHeightMm(QString str);
    void SaveAndExit();
    void FineAdjust();
    void sltChangeDitheringBits(int index);
    void sltTrackPrintableZone(QRect& rect);
    void sltShowNewSelection();
    void sltGenNewSelection();
    void Exit();

private:
    Ui::CorteImagen *ui;
    QPixmap m_image;
    //QLabelExt *m_imageLabel;
    QPoint m_initPoint;
    QPoint m_initSmallPoint;
    int m_selectedW;
    int  m_selectedH;
    int m_zWidth;
    int m_zHeight;
    int m_verticalZoomOffset;
    bool m_locked;
    In2GraphicsScene *m_scene;
    In2GraphicsView *m_In2GView;
    MemoryMap *m_memoryMap;

    //QWidget *m_parent;
};

#endif // CORTEIMAGEN_H
