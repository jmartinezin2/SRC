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
#include "qlabelext.h"

#define APPROACH_LEN 5

enum CORNER_ENUM{
    CORNER_NONE=0,
    CORNER_TOPLEFT,
    CORNER_TOPRIGHT,
    CORNER_BOTTOMLEFT,
    CORNER_BOTTOMRIGHT
};

QLabelExt::QLabelExt(QWidget *parent) :
    QLabel(parent)
{
    memset(&pulsacion,0x00,sizeof(pulsacion));
    rubberBand=NULL;
    cornerRubberBand=NULL;
    m_blocked=false;
    m_CornerSelected=CORNER_NONE;
}

QLabelExt::~QLabelExt()
{
}

void QLabelExt::enterEvent ( QEvent * event )
{
}

void QLabelExt::leaveEvent ( QEvent * event )
{
}
void QLabelExt::paintRubber(const QRect &r){
    if (!m_blocked){
        if (this->pixmap()){
            rubberBand->setGeometry(r);
       }
    }
}

void QLabelExt::mouseMoveEvent ( QMouseEvent * event )
{
    if (!m_blocked){
        this->paintRubber(QRect(origin, event->pos()).normalized());
        emit moving(event->pos());
    }//Si está bloqueado, busco vertices.
    else{
        //Si está pulsado, modifico el rubberband
        if (pulsacion.pulso){
            modifyRubberBand(event->pos());
        }
        //Si está libre, busco vertices
        else{
            lookRubberCorner(event->pos());
        }
    }
}

void QLabelExt::mousePressEvent ( QMouseEvent * event )
{
    if (!m_blocked){
        if (this->pixmap()){
            if (event->button() == Qt::LeftButton) {
                pulsacion.x1 = event->x();
                pulsacion.y1 = event->y();
                origin = event->pos();

                if (!rubberBand) rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                rubberBand->setGeometry(QRect(origin, QSize()));
                rubberBand->show();
                emit press(event->pos());
            }
        }
    }
    pulsacion.pulso=true;
}
/** Evento de release*/
void QLabelExt::mouseReleaseEvent ( QMouseEvent * event )
{
    emit release(event->pos());
    pulsacion.pulso=false;
    m_CornerSelected=CORNER_NONE;
}

/** Bloquea el rubberband*/
void QLabelExt::SetBlocked(bool block){
    m_blocked=block;
    this->setMouseTracking(block); //Si está bloqueado, hacemos tracking del mouse para testear vertices
    if (m_blocked){
        if (rubberBand){
            QPalette pal;
            pal.setBrush(QPalette::Highlight, QBrush(Qt::blue));
            //pal.setBrush(QPalette::Base, QBrush(Qt::red));
            rubberBand->setPalette(pal);
            rubberBand->show();
        }
        else{
            /*QPalette pal;
            pal.setBrush(QPalette::Foreground, QBrush(Qt::gray));
            pal.setBrush(QPalette::Base, QBrush(Qt::gray));
            rubberBand->setPalette(pal);
            rubberBand->show();*/
        }
    }
}

/** Devuelve si está bloqueado*/
bool QLabelExt::isBlocked(){
    return m_blocked;
}

/** Dibuja el rubber*/
void QLabelExt::setRubber(const QRect & rect){
    if (!rubberBand) rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    QRect rectMod=rect;
    if ((rect.x()+rect.width())>=this->x()+this->width()){
        rectMod.setWidth(this->x()+this->width()-rect.x()-1);
    }
    if ((rect.y()+rect.height())>=this->y()+this->height()){
        rectMod.setHeight(this->y()+this->height()-rect.y()-1);
    }

    this->paintRubber(rectMod);
    rubberBand->show();
    origin=rect.topLeft();
}

/** Busca vertices*/
void QLabelExt::lookRubberCorner(const QPoint &p){
    if (rubberBand){
        QRect rub=rubberBand->geometry();
        //Testeo primero en vertical
        if (abs(p.y()-rub.top())<APPROACH_LEN){
            if (abs(p.x()-rub.left())<APPROACH_LEN){
                m_CornerSelected=CORNER_TOPLEFT;
            }
            else if (abs(p.x()-rub.right())<APPROACH_LEN){
                m_CornerSelected=CORNER_TOPRIGHT;
            }
            else m_CornerSelected=CORNER_NONE;
        }
        else if (abs(p.y()-rub.bottom())<APPROACH_LEN){
            if (abs(p.x()-rub.left())<APPROACH_LEN){
                m_CornerSelected=CORNER_BOTTOMLEFT;
            }
            else if (abs(p.x()-rub.right())<APPROACH_LEN){
                m_CornerSelected=CORNER_BOTTOMRIGHT;
            }
            else m_CornerSelected=CORNER_NONE;
        }
        else{
            m_CornerSelected=CORNER_NONE;
        }
    }
    else m_CornerSelected=CORNER_NONE;
}

/** Modifica el rubberBand*/
void QLabelExt::modifyRubberBand(const QPoint &p){
    if (rubberBand){
        QRect rectShape=rubberBand->geometry();
        switch (m_CornerSelected){
            case CORNER_NONE:
                break;
            case CORNER_TOPLEFT:
                rectShape.setTopLeft(p);
                rubberBand->setGeometry(rectShape);
                emit newSelection(rectShape);
                break;
            case CORNER_TOPRIGHT:
                rectShape.setTopRight(p);
                rubberBand->setGeometry(rectShape);
                emit newSelection(rectShape);
                break;
            case CORNER_BOTTOMLEFT:
                rectShape.setBottomLeft(p);
                rubberBand->setGeometry(rectShape);
                emit newSelection(rectShape);
                break;
            case CORNER_BOTTOMRIGHT:
                rectShape.setBottomRight(p);
                rubberBand->setGeometry(rectShape);
                emit newSelection(rectShape);
                break;
            }
    }

}

