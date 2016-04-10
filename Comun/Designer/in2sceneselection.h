#ifndef IN2SCENESELECTION_H
#define IN2SCENESELECTION_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class In2SceneSelection :public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit In2SceneSelection(QGraphicsPixmapItem *parent = 0,int ID=0,int color=Qt::red);
    void generatePrintZone(QRect &rect);
    void generatePrintZoneExt(QRect &rect,float angle,QColor color);

    int getID();
    void setColor(int color);

signals:
    void sgnRect(int id,QRect &rect);

private:
    void generateNewPrintZone(QRect & rect);
    void generateNewPrintZoneExt(QRect & rect,float angle, QColor color);
    void sltAddItem();


private:
    QPixmap m_pixmap;
    bool m_pressed;
    int m_vortex;
    int m_id;
    int m_color;

protected:
 void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
 void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
 void mousePressEvent ( QGraphicsSceneMouseEvent * event );

    
public slots:
    
};

#endif // IN2SCENESELECTION_H
