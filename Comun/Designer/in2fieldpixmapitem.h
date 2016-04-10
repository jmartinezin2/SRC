#ifndef IN2FIELDPIXMAPITEM_H
#define IN2FIELDPIXMAPITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>

class In2FieldPixmapItem : public QObject,public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    In2FieldPixmapItem(QGraphicsPixmapItem *parent=0,int id=0);
    ~In2FieldPixmapItem();
    bool isSelected();
    int getID();

protected:
 void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
 void mousePressEvent ( QGraphicsSceneMouseEvent * event );

signals:
    void sgnFieldPressed(int id);
    void sgnFieldReleased(int id);

private:
    bool m_selected;
    int m_id;
};

#endif // IN2FIELDPIXMAPITEM_H
