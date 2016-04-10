#include "in2pixmapcomposer.h"
#include <QPainter>

In2PixmapComposer::In2PixmapComposer(QObject *parent) :
    QObject(parent)
{
    m_vector.clear();
}

/** Añade un item a la lista*/
void In2PixmapComposer::addItem(const In2PixmapComposerItem &p){
    In2PixmapComposerItem item=p;
    m_vector.append(item);
}

/** Reemplaza un item de la lista*/
void In2PixmapComposer::replaceItem(int index, const In2PixmapComposerItem &p){
    if (index<m_vector.count()){
        m_vector.replace(index,p);
    }
}
/** Devuelve un item*/
In2PixmapComposerItem In2PixmapComposer::getItem(int index){
    if (index<m_vector.count()){
        return m_vector.at(index);
    }
    else return In2PixmapComposerItem();
}


/** Cambia un pixmap*/
void In2PixmapComposer::replacePixmap(int index, const QPixmap &p){
    if (index<m_vector.count()){
        In2PixmapComposerItem item=m_vector.at(index);
        item.setPixmap(p);
        m_vector.replace(index,item);
    }
}

/** Cambia un punto*/
void In2PixmapComposer::replacePoint(int index, QPoint p){
    if (index<m_vector.count()){
        In2PixmapComposerItem item=m_vector.at(index);
        item.setPoint(p);
        m_vector.replace(index,item);
    }
}

/** Cambia el repaint*/
void In2PixmapComposer::replaceRepaint(int index, bool repaint){
    if (index<m_vector.count()){
        In2PixmapComposerItem item=m_vector.at(index);
        item.setRepaint(repaint);
        m_vector.replace(index,item);
    }
}

/** Renderiza*/
QPixmap In2PixmapComposer::render(){
    if (m_vector.count()){
        In2PixmapComposerItem item=m_vector.at(0);
        QPixmap lienzo(item.getPixmap());
        QPainter p(&lienzo);
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        for (int x=0;x<m_vector.count();x++){
            In2PixmapComposerItem item=m_vector.at(x);
            if (item.getRepaint()){
               p.drawPixmap(item.getPoint(),item.getPixmap());
            }
        }
        return lienzo;


    }
    else return QPixmap();
}
