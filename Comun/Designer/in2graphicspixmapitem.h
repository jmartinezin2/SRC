#ifndef IN2GRAPHICSPIXMAPITEM_H
#define IN2GRAPHICSPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QPen>

//Estilo de los elementos moveables
typedef enum styles{
    CMYK_STYLE=0,
    CUT_STYLE
}STYLES;

class In2GraphicsPixmapItem :public QObject, public QGraphicsPixmapItem

{
    Q_OBJECT

public:
    explicit In2GraphicsPixmapItem(QGraphicsPixmapItem *parent=0,int id=0,bool changeBack=false);
    void setID(int i);
    int getID();
    void setPixmap(const QPixmap &pixmap);
    bool isSelected();
    void enableShowPrintableZone(bool enable);
    void setStyle(int style);
    void generateNewPrintZone(QRect &rect);
    void setMaximumPrintZone(int max);
    void generatePrintZone(QRect &rect);




protected:
 void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
 void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
 void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
 void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
 void mousePressEvent ( QGraphicsSceneMouseEvent * event );
 //QVariant itemChange(GraphicsItemChange change, const QVariant &value);


 void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


private:
 QPen m_pen;
 QPixmap m_selPixmap;
 QPixmap m_printPixmap;
 QPixmap m_originalPixmap;
 bool m_selected;
 bool m_changeBackGround;
 int m_ID;
 bool m_pressed;
 bool m_enablePrintZone;
 int m_maxWidth;
 int m_style;
 int m_vortex;
    
signals:
 //void sgnReleasedIn2GraphicsPixmapItem();
 void sgnNewRect(QRect & rect);

public slots:
    
};

#endif // IN2GRAPHICSPIXMAPITEM_H
