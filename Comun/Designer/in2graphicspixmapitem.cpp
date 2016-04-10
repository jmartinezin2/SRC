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

#include "in2graphicspixmapitem.h"
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QSize>
#include <QWidget>
#include <QBitmap>
#include <QRect>
#include <QGraphicsScene>
#include "in2defines.h"
//#include "globals.h"

//Anchura por defecto del printbar
#define PRINTBAR_H 120
//Disntancia manhattan
#define MANHATTAN_LEN 50

//Tamaño de la  marca
#define MARK MANHATTAN_LEN

/** Creator*/
In2GraphicsPixmapItem::In2GraphicsPixmapItem(QGraphicsPixmapItem *parent,int id,bool changeBack) :
    QGraphicsPixmapItem(parent)
{
     if (changeBack) setAcceptHoverEvents(true);
     else setAcceptHoverEvents(false);

     m_selected=false;
     m_ID=id;
     m_changeBackGround=changeBack;
     m_enablePrintZone=false;
     m_maxWidth=DEFAULT_HEAD_WIDTH;
     m_pressed=false;
     m_style=CMYK_STYLE;
     m_vortex=0;


}
/** Carga el ID*/
void In2GraphicsPixmapItem::setID(int i){
    m_ID=i;
}
/** Devuelve el ID*/
int In2GraphicsPixmapItem::getID(){
    return m_ID;
}

/** Sobrecarga el setPixmap y añade mascara de color*/
void In2GraphicsPixmapItem::setPixmap(const QPixmap &pixmap){
    if (m_changeBackGround){
        this->m_originalPixmap=pixmap;
        this->m_selPixmap=QPixmap(pixmap);
        this->m_selPixmap.fill(QColor(255,255,255,1).rgb());
        QBitmap mask=pixmap.createMaskFromColor(Qt::black,Qt::MaskOutColor);
        QPainter p(&this->m_selPixmap);
        p.setPen(QColor(Qt::red));
        p.drawPixmap(this->m_selPixmap.rect(),mask,mask.rect());
        p.end();
        this->m_selPixmap.setMask(mask);
    }
    QGraphicsPixmapItem::setPixmap(pixmap);
}

/** Cuando pasamos raton por encima*/
void In2GraphicsPixmapItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_changeBackGround){
        if (!m_selected){
            QGraphicsPixmapItem::setPixmap(m_selPixmap);
            this->setCursor(QCursor(Qt::PointingHandCursor));
            m_selected=true;
            update();
        }
    }
}
/** Cuando salimos de encima*/
void In2GraphicsPixmapItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_changeBackGround){
        if (m_selected){
            QGraphicsPixmapItem::setPixmap(m_originalPixmap);
            this->setCursor(QCursor(Qt::ArrowCursor));
            m_selected=false;
            update();
        }
    }
}
/** Habilita Generar un cuadrado semitransparente*/
void In2GraphicsPixmapItem::enableShowPrintableZone(bool enable){
    m_enablePrintZone=enable;
}
/** Genera un cuadrado semitransparente*/
void In2GraphicsPixmapItem::generateNewPrintZone(QRect &rect){
    if (m_style==CMYK_STYLE){
        QPixmap dummy(m_maxWidth,rect.height());
        dummy.fill(QColor(255,255,255,1).toRgb());
        QPainter p(&dummy);
        p.fillRect(rect,QColor(0,0,125,75));
        QRect band=rect;
        band.setTop(rect.y()+rect.height()-MANHATTAN_LEN);
        p.fillRect(band,QColor(100,100,100,75));

        QRect printBar(0,0,m_maxWidth,PRINTBAR_H);
        QRect printHeadC(0,0,m_maxWidth,PRINTBAR_H/4);
        QRect printHeadM(0,PRINTBAR_H/4,m_maxWidth,PRINTBAR_H/4);
        QRect printHeadY(0,2*PRINTBAR_H/4,m_maxWidth,PRINTBAR_H/4);
        QRect printHeadB(0,3*PRINTBAR_H/4,m_maxWidth,PRINTBAR_H/4);

        QRadialGradient gradient(QPoint(m_maxWidth/2,printHeadC.y()+PRINTBAR_H/8),m_maxWidth/2); // diagonal gradient from top-left to bottom-right
        QColor color1=QColor(Qt::cyan);
        QColor color2=QColor(Qt::white);
        color1.setAlpha(75);
        color2.setAlpha(75);
        gradient.setColorAt(0, color1);
        gradient.setColorAt(1, color2);
        p.fillRect(printHeadC,gradient);
        gradient.setCenter(QPoint(m_maxWidth/2,printHeadM.y()+PRINTBAR_H/8));
        color1=QColor(Qt::magenta);
        gradient.setColorAt(0, color1);
        gradient.setColorAt(1, color2);
        p.fillRect(printHeadM,gradient);
        gradient.setCenter(QPoint(m_maxWidth/2,printHeadY.y()+PRINTBAR_H/8));
        color1=QColor(Qt::yellow);
        gradient.setColorAt(0, color1);
        gradient.setColorAt(1, color2);
        p.fillRect(printHeadY,gradient);
        gradient.setCenter(QPoint(m_maxWidth/2,printHeadB.y()+PRINTBAR_H/8));
        color1=QColor(Qt::black);
        gradient.setColorAt(0, color1);
        gradient.setColorAt(1, color2);
        p.fillRect(printHeadB,gradient);

        QPen pen(Qt::black,10);
        pen.setCosmetic(true);
        p.setPen(pen);
        p.drawRect(printBar);
        p.end();
        setPixmap(dummy);
        m_printPixmap=dummy;
    }
    else if (m_style==CUT_STYLE){
        QPixmap dummy(m_maxWidth,rect.height());
        dummy.fill(QColor(255,255,255,1).toRgb());
        QPainter p(&dummy);
        p.fillRect(rect,QColor(0,0,125,75));

        /*QPoint point=rect.topLeft();
        QRect pRect=QRect(point,QSize(MARK,MARK));
        p.fillRect(pRect,QColor(0,125,0,75));

        point=rect.bottomLeft()+QPoint(0,-MARK);
        pRect=QRect(point,QSize(MARK,MARK));
        p.fillRect(pRect,QColor(0,125,0,75));

        point=rect.topRight()+QPoint(-MARK,0);
        pRect=QRect(point,QSize(MARK,MARK));
        p.fillRect(pRect,QColor(0,125,0,75));

        point=rect.bottomRight()+QPoint(-MARK,-MARK);
        pRect=QRect(point,QSize(MARK,MARK));
        p.fillRect(pRect,QColor(0,125,0,75));*/
        QPen pen(Qt::blue,15,Qt::DashLine,Qt::RoundCap,Qt::RoundJoin);
        pen.setCosmetic(true);
        p.setPen(pen);
        p.drawLine(rect.topLeft(),QPoint(rect.x()+rect.height()/10,rect.y()));
        p.drawLine(rect.topLeft(),QPoint(rect.x(),rect.y()+rect.height()/10));
        p.drawLine(rect.topRight(),QPoint(rect.topRight().x(),rect.y()+rect.height()/10));
        p.drawLine(rect.topRight(),QPoint(rect.topRight().x()-rect.height()/10,rect.y()));
        p.drawLine(rect.bottomRight(),QPoint(rect.bottomRight().x()-rect.height()/10,rect.bottomRight().y()));
        p.drawLine(rect.bottomRight(),QPoint(rect.bottomRight().x(),rect.bottomRight().y()-rect.height()/10));
        p.drawLine(rect.bottomLeft(),QPoint(rect.bottomLeft().x(),rect.bottomLeft().y()-rect.height()/10));
        p.drawLine(rect.bottomLeft(),QPoint(rect.bottomLeft().x()+rect.height()/10,rect.bottomLeft().y()));

        p.end();
        setPixmap(dummy);
        m_printPixmap=dummy;
    }

}



/** Genera un cuadrado semitransparente*/
void In2GraphicsPixmapItem::generatePrintZone(QRect &rect){

    if (m_printPixmap.isNull()){
        generateNewPrintZone(rect);
    }
    else{
        QPixmap dummy=m_printPixmap.scaled(rect.size());
        setPixmap(dummy);
    }

}
/** Estrablece zona maxima de impresion*/
void In2GraphicsPixmapItem::setMaximumPrintZone(int max){
    m_maxWidth=max;
}

/** cuando movemos el mouse...generamos un cuadrado si se permite*/
void In2GraphicsPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if (m_enablePrintZone){
        QRect currentRect=this->pixmap().rect();//this->boundingRect().toRect();
        QPoint topLeft=currentRect.topLeft();
        QPoint bottomRight=currentRect.bottomRight();
        QPoint bottomLeft=currentRect.bottomLeft();
        QPoint topRight=currentRect.topRight();
        QPoint eventPoint=event->pos().toPoint();       
        int y=bottomRight.y();
        int w=currentRect.width();
        int h=currentRect.height();

        if (m_style==CMYK_STYLE){
            if ((m_pressed)||(qAbs(y-eventPoint.y())<MANHATTAN_LEN)){
                QRect newRect(topLeft,eventPoint);
                if (newRect.x()<0) newRect.setX(0);
                if (newRect.y()<0) newRect.setY(0);
                if (newRect.width()>m_maxWidth) newRect.setWidth(m_maxWidth);
                if (newRect.width()<0) newRect.setWidth(0);
                if (newRect.height()<0) newRect.setHeight(0);
                //if (newRect.height()>this->pixmap().height()) newRect.setHeight(this->pixmap().height());
                generatePrintZone(newRect);
                m_pressed=true;
                emit sgnNewRect(newRect);
                return;
            }
        }
        else if (m_style==CUT_STYLE){
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
             if (newRect.width()>m_maxWidth) newRect.setWidth(m_maxWidth);
             if (newRect.width()<0) newRect.setWidth(0);
             if (newRect.height()<0) newRect.setHeight(0);
             //if (newRect.height()>this->pixmap().height()) newRect.setHeight(this->pixmap().height());
             generatePrintZone(newRect);
             m_pressed=true;
             emit sgnNewRect(newRect);
             return;
         }
         else if (m_pressed){
             emit sgnNewRect(newRect);
         }
        }
    }
    QGraphicsPixmapItem::mouseMoveEvent(event);

}

/** Carga un estilo*/
void In2GraphicsPixmapItem::setStyle(int style){
    m_style=style;
}

/** Cuando se deja de presionar el mouse*/
void In2GraphicsPixmapItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ){
    m_pressed=false;
    m_vortex=0;
    QGraphicsPixmapItem::mouseReleaseEvent(event);

}

/** Cuando se presiona el mouse*/
void In2GraphicsPixmapItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ){
    if (m_style==CUT_STYLE){
        m_pressed=true;
    }
    QGraphicsPixmapItem::mousePressEvent(event);

}


/** Sobrecarga el paint*/
void In2GraphicsPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
        QGraphicsPixmapItem::paint(painter,option,widget);
}

/** Devuelve si el item está seleccionado o no*/
bool In2GraphicsPixmapItem::isSelected(){
    return m_selected;
}

/** Mantiene el cuadro dentro de la escena*/
/*QVariant In2GraphicsPixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}*/
