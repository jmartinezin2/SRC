#ifndef IN2GRAPHICSSCENE_H
#define IN2GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QVarLengthArray>
#include <QPainter>
#include <QRubberBand>
#include "in2graphicspixmapitem.h"
#include "in2fieldpixmapitem.h"

class In2GraphicsScene : public QGraphicsScene
{
public:
   //In2GraphicsScene(QObject *parent = 0);
   //In2GraphicsScene(QRect rect);
    In2GraphicsScene(qreal x, qreal y, qreal w, qreal h);
    ~In2GraphicsScene();    
    In2GraphicsPixmapItem* getItemSelected();
    In2FieldPixmapItem *getFieldSelected();
    In2FieldPixmapItem * getFieldSelectedFromID(int id);
    void setGridSize(float size);
    float getGridSize();
    QVector<QLineF> getGridLines();
    bool isGridEnabled();
signals:

protected:
    virtual void drawForeground(QPainter *painter, const QRectF &rect);    

private:
    bool m_benableGrid;
    float m_gridSize;
    QVector<QLineF> lines;
    QVector <QGraphicsLineItem *> m_grid;
    QVector <QGraphicsTextItem *> m_text;

public slots:
    void enableGrid(bool value);
};

#endif // IN2GRAPHICSSCENE_H
