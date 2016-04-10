#ifndef IN2PIXMAPCOMPOSERITEM_H
#define IN2PIXMAPCOMPOSERITEM_H

#include <QObject>
#include <QPixmap>
#include <QPoint>

class In2PixmapComposerItem : public QObject
{
    Q_OBJECT
public:
    In2PixmapComposerItem(QObject *parent = 0);
    In2PixmapComposerItem( const In2PixmapComposerItem &s );
    In2PixmapComposerItem& operator=( const In2PixmapComposerItem &s ) {return *this;} // assignment operator
    void setItem(const QPixmap &pixmap,QPoint p, bool repaint);

    void setPixmap(const QPixmap &);
    void setRepaint(bool repaint);
    void setPoint(QPoint point);

    QPixmap getPixmap();
    bool getRepaint();
    QPoint getPoint();



public:
    QPixmap m_pixmap;
    QPoint m_point;
    bool m_bRepaint;
    
signals:
    
public slots:
    
};

#endif // IN2PIXMAPCOMPOSERITEM_H
