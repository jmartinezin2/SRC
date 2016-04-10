/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "in2sceneselection.h"
#include <QPainter>

In2SceneSelection::In2SceneSelection(QGraphicsPixmapItem *parent,int ID, int color) :
    QGraphicsPixmapItem(parent)
{
    m_pressed=false;
    m_vortex=0;
    m_id=ID;
    m_color=color;
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
}


/** Carga un color*/
void In2SceneSelection::setColor(int color){
    m_color=color;
}

/** Devuelve el ID*/
int In2SceneSelection::getID(){
    return m_id;
}

/** Genera un cuadrado semitransparente*/
void In2SceneSelection::generatePrintZone(QRect &rect){

    if (m_pixmap.isNull()){
        generateNewPrintZone(rect);
    }
    else{
        QPixmap dummy=m_pixmap.scaled(rect.size());
        setPixmap(dummy);
    }
    QRect foo=QRect(this->scenePos().x(),this->scenePos().y(),this->pixmap().width(),this->pixmap().height());
    emit sgnRect(m_id,foo);

}

/** Genera un cuadrado semitransparente*/
void In2SceneSelection::generatePrintZoneExt(QRect &rect,float angle,QColor color){

    if (m_pixmap.isNull()){
        generateNewPrintZoneExt(rect,angle,color);
    }
    else{
        QPixmap dummy=m_pixmap.scaled(rect.size());
        setPixmap(dummy);
    }
    QRect foo=QRect(this->scenePos().x(),this->scenePos().y(),this->pixmap().width(),this->pixmap().height());
    emit sgnRect(m_id,foo);

}

/** Genera un cuadrado semitransparente ampliado*/
void In2SceneSelection::generateNewPrintZoneExt(QRect &rect,float angle,QColor color){
    QPixmap foo=QPixmap(rect.width(),rect.height());
    foo.fill(QColor(255,255,255,1).toRgb());
    QPainter p(&foo);
    p.fillRect(rect,color);
    QPen pen(Qt::red,5);
    p.setPen(pen);
    p.drawRect(rect);
    pen=QPen(Qt::blue,5);
    p.setPen(pen);
    QFont font;
    font.setPointSize(30);
    font.setBold(false);
    p.setFont(font);

    QRect rect2=QRect(QPoint(0,0),QSize(rect.width()/4,rect.height()));
    p.drawText(rect2,Qt::AlignTop,QString("%1").arg(m_id+1));
    m_pixmap=foo.transformed(QTransform().rotate(angle));
    setPixmap(m_pixmap);

}




/** Genera un cuadrado semitransparente*/
void In2SceneSelection::generateNewPrintZone(QRect &rect){
    m_pixmap=QPixmap(rect.width(),rect.height());
    m_pixmap.fill(QColor(255,255,255,1).toRgb());
    QPainter p(&m_pixmap);
    p.fillRect(rect,QColor(0,0,125,75));
    QPen pen(Qt::red,2);
    p.setPen(pen);
    p.drawRect(rect);
    pen=QPen(Qt::black,5);
    p.setPen(pen);
    QFont font;
    font.setPointSize(25);
    font.setBold(true);
    p.setFont(font);

    QRect rect2=QRect(QPoint(0,0),QSize(rect.width()/4,rect.height()));
    p.drawText(rect2,Qt::AlignTop,QString("%1").arg(m_id+1));

    setPixmap(m_pixmap);

}

/** Evento de move*/
void In2SceneSelection::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if (m_pressed){
        QRect currentRect=this->pixmap().rect();//this->boundingRect().toRect();
        QPoint topLeft=currentRect.topLeft();
        QPoint bottomRight=currentRect.bottomRight();
        QPoint bottomLeft=currentRect.bottomLeft();
        QPoint topRight=currentRect.topRight();
        QPoint eventPoint=event->pos().toPoint();
        //int y=bottomRight.y();
        int w=currentRect.width();
        int h=currentRect.height();

        QRect newRect;
        bool m_resize=false;
        int manhattan_len=this->pixmap().height()/10;
        QPoint ref=eventPoint-topLeft;

        if ((m_vortex==1)||(ref.manhattanLength()<manhattan_len)){
             m_vortex=1;
             m_resize=true;
             this->moveBy(eventPoint.x(),eventPoint.y());
             if (eventPoint.x()>0) w-=eventPoint.x();
             else w-=2*eventPoint.x();
             if (eventPoint.y()>0) h-=eventPoint.y();
             else h-=2*eventPoint.y();
             /*w-=qAbs(eventPoint.x());
             h-=qAbs(eventPoint.y());*/
             newRect=QRect(eventPoint.x(),eventPoint.y(),w,h);
        }
        else{
            ref=eventPoint-topRight;
            if ((m_vortex==2)||(ref.manhattanLength()<manhattan_len)){
                m_vortex=2;
                m_resize=true;
                this->moveBy(0,eventPoint.y());
                if (eventPoint.x()>0) w-=eventPoint.x();
                else w-=2*eventPoint.x();
                if (eventPoint.y()>0) h-=eventPoint.y();
                else h-=2*eventPoint.y();
                newRect=QRect(bottomLeft.x(),eventPoint.y(),eventPoint.x(),h);
            }
            else{
                ref=eventPoint-bottomLeft;
                if ((m_vortex==3)||(ref.manhattanLength()<manhattan_len)){
                    m_vortex=3;
                    m_resize=true;
                    this->moveBy(eventPoint.x(),0);
                    if (eventPoint.x()>0) w-=eventPoint.x();
                    else w-=2*eventPoint.x();
                    newRect=QRect(eventPoint.x(),topLeft.y(),w,eventPoint.y());
                }
                else{
                    ref=eventPoint-bottomRight;
                    if ((m_vortex==4)||(ref.manhattanLength()<manhattan_len)){
                        m_vortex=4;
                        m_resize=true;
                        if (eventPoint.x()>0) w-=eventPoint.x();
                        else w-=2*eventPoint.x();
                        newRect=QRect(topLeft,eventPoint);
                    }
                }
             }
          }
     if (m_resize){
         if (newRect.x()<0) newRect.setX(0);
         if (newRect.y()<0) newRect.setY(0);
         if (newRect.width()<0) newRect.setWidth(0);
         if (newRect.height()<0) newRect.setHeight(0);
         generatePrintZone(newRect);
         m_pressed=true;
         return;
     }
     else{
         QRect foo=QRect(this->scenePos().x(),this->scenePos().y(),this->pixmap().width(),this->pixmap().height());
         emit sgnRect(m_id,foo);
     }
   }
    QGraphicsPixmapItem::mouseMoveEvent(event);
}
/** Evento de press*/
void In2SceneSelection::mousePressEvent(QGraphicsSceneMouseEvent *event){
    m_pressed=true;
    QGraphicsPixmapItem::mousePressEvent(event);
}
/** Evento de release*/
void In2SceneSelection::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    m_pressed=false;
    m_vortex=0;
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}
