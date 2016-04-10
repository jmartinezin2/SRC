#include "in2pixmapcomposeritem.h"

In2PixmapComposerItem::In2PixmapComposerItem(QObject *parent) :
    QObject(parent)
{

    m_point=QPoint(0,0);
    m_bRepaint=false;
}
/** Copy constructor*/
In2PixmapComposerItem::In2PixmapComposerItem(const In2PixmapComposerItem &s){
    m_pixmap=s.m_pixmap;
    m_point=s.m_point;
    m_bRepaint=s.m_bRepaint;
}

/** con todos componentes*/
void In2PixmapComposerItem::setItem(const QPixmap &pixmap, QPoint p, bool repaint){
    m_pixmap=pixmap;
    m_point=p;
    m_bRepaint=repaint;
}

/** Cambia el pixmap*/
void In2PixmapComposerItem::setPixmap(const QPixmap & p){
    m_pixmap=p;
}
/** Cambia el repaint*/
void In2PixmapComposerItem::setRepaint(bool repaint){
    this->m_bRepaint=repaint;
}
/** Cambia el punto*/
void In2PixmapComposerItem::setPoint(QPoint point){
    m_point=point;
}

/** Devuelve el Pixmap*/
QPixmap In2PixmapComposerItem::getPixmap(){
    return m_pixmap;
}

/** Devuelve el repaint*/
bool In2PixmapComposerItem::getRepaint(){
    return m_bRepaint;
}

/** Devuelve el punto*/
QPoint In2PixmapComposerItem::getPoint(){
    return m_point;
}


