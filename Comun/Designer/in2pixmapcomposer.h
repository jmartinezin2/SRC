#ifndef IN2PIXMAPCOMPOSER_H
#define IN2PIXMAPCOMPOSER_H

#include <QObject>
#include <QPixmap>
#include <QPoint>
#include "in2pixmapcomposeritem.h"

class In2PixmapComposer : public QObject
{
    Q_OBJECT
public:
    explicit In2PixmapComposer(QObject *parent = 0);
    void addItem(const In2PixmapComposerItem &p);
    void replaceItem(int index,const In2PixmapComposerItem & p);
    In2PixmapComposerItem getItem(int index);
    void replacePixmap(int index, const QPixmap & p);
    void replacePoint(int index,QPoint p);
    void replaceRepaint(int index,bool repaint);

    QPixmap render();

signals:

private:
    QVector<In2PixmapComposerItem> m_vector;
public slots:
    
};

#endif // IN2PIXMAPCOMPOSER_H
