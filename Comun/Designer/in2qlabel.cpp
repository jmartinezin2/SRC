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
#include "in2qlabel.h"
#include <QImage>
#include <QPainter>
#include <QDrag>
#include <QMimeData>

#define     IMAGEN_SINESCALAR  0
#define     IMAGEN_S2W         1
#define     IMAGEN_S2H         2


In2QLabel::In2QLabel(QWidget * parent) : QLabel(parent)
{

    //this->ImageCopy=this->pixmap();
    this->setMouseTracking(true);
    memset(&pulsacion,0x00,sizeof(pulsacion));
    rubberBand=NULL;
    m_pressed=false;
    // Si existe, hacemos una copia del original
    if (this->pixmap()){
       this->m_PictureOriginal=this->pixmap()->copy(this->pixmap()->rect());
    }
}
/** Movemos el objeto*/
void In2QLabel::dragEnterEvent(QDragEnterEvent *event){
    if (event->mimeData()->hasText()) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
    emit LabelPos(event->pos());
}

void In2QLabel::dropEvent(QDropEvent *event)
{
    //Calculamos el punto exacto
    QPoint hotSpot;
    const QMimeData *mime = event->mimeData();
    QList<QByteArray> hotSpotPos = mime->data("application/x-hotspot").split(' ');
    if (hotSpotPos.size() == 2) {
        hotSpot.setX(hotSpotPos.first().toInt());
        hotSpot.setY(hotSpotPos.last().toInt());
    }
    emit SetItemPos(event->pos()-hotSpot);


    this->SetAcceptDrops(false);


}



void In2QLabel::SetAcceptDrops(bool accept){
    this->setAcceptDrops(accept);
}

In2QLabel::~In2QLabel()
{
}

void In2QLabel::enterEvent ( QEvent * event )
{
}

void In2QLabel::leaveEvent ( QEvent * event )
{
}

void In2QLabel::mouseMoveEvent ( QMouseEvent * event )
{
    if (m_pressed){
        if (this->pixmap()){
            if (rubberBand) rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
        }
    }
    else{
            emit LabelPos(event->pos());
    }
}
/** Evento de press..retrocedemos en el zoom si se hace con el derecho*/
void In2QLabel::mousePressEvent ( QMouseEvent * event )
{
    if (this->acceptDrops()){
        QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
        if (!child)
            return;

        QPoint hotSpot = event->pos() - child->pos();

        QMimeData *mimeData = new QMimeData;
        mimeData->setText(child->text());
        mimeData->setData("application/x-hotspot",
                          QByteArray::number(hotSpot.x())
                          + " " + QByteArray::number(hotSpot.y()));



        QPixmap pixmap(child->size());
        child->render(&pixmap);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(hotSpot);

        Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);

        if (dropAction == Qt::MoveAction)
            child->close();

    }
    else{
        m_pressed=true;
        if (this->pixmap()){
            if (event->button() == Qt::LeftButton) {
                pulsacion.x1 = event->x();
                pulsacion.y1 = event->y();
                //pulsacion.zooming = true;

                origin = event->pos();

                if (!rubberBand) rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                rubberBand->setGeometry(QRect(origin, QSize()));
                rubberBand->show();
            }
            else if (event->button() == Qt::RightButton) {
                if (prevPixmap.count()>0){
                    this->setPixmap(prevPixmap.takeLast(),false);
                //prevPixmap.removeLast();
                }
            }
        }
    }
}
/** Evento de release*/
void In2QLabel::mouseReleaseEvent ( QMouseEvent * event )
{
    m_pressed=false;
    if (this->pixmap()){
            if (event->button() == Qt::LeftButton ) {
                rubberBand->hide();
                pulsacion.x2 = event->x();
                pulsacion.y2 = event->y();
                //Si hacemos zoom de abajo a arriba, retrocedemos
                if (pulsacion.y1>pulsacion.y2){
                    if (prevPixmap.count()>0){
                        this->setPixmap(prevPixmap.takeLast(),false);
                        //prevPixmap.removeLast();
                    }
                    pulsacion.zooming=false;
                }
                //Si no es doble pulsacion...
                else if ((event->x()!=origin.x())&&(event->y()!=origin.y())&&(pulsacion.zooming==false)){
                    pulsacion.zooming=true;
                    QPixmap pic(this->size());
                    QPainter p(&pic);   // create a QPainter for it

                    QLinearGradient lg(0, 0, 0, this->height());
                    lg.setColorAt(0, Qt::gray);
                    //lg.setColorAt(0, QColor(18,18,18));
                    lg.setColorAt(1, Qt::lightGray);
                    p.setPen(Qt::NoPen);
                    p.setBrush(lg);
                    p.drawRect(this->rect());

                    int width=pulsacion.x2-pulsacion.x1;
                    int heigth=pulsacion.y2-pulsacion.y1;
                    this->setMaximumWidth(this->width());
                    this->setMaximumHeight(this->height());
                    QRect rect=QRect(pulsacion.x1,pulsacion.y1,width,heigth);
                    //QPixmap Imagen=this->pixmap()->copy(rect);
                    QPixmap Imagen;
                    if (!m_PictureOriginal.isNull()){
                    //El label es mas ancho
                        if ((1000*this->width()/this->height())>=(1000*m_PictureOriginal.width()/m_PictureOriginal.height())){
                            //int newHeight=this->height();
                            int newWidth=(this->height()*m_PictureOriginal.width())/m_PictureOriginal.height();
                            int Offset=(this->width()-newWidth)/2;
                            float Proporcion=float(m_PictureOriginal.height())/float(this->height());
                            QRect dummyrect=QRect((pulsacion.x1-Offset)*1000*Proporcion/1000,(pulsacion.y1)*1000*Proporcion/1000,width*1000*Proporcion/1000,heigth*1000*Proporcion/1000);
                            rect=dummyrect;
                        }
                        else{
                            //int newWidth=this->width();
                            int newHeight=(this->width()*m_PictureOriginal.height())/m_PictureOriginal.width();
                            int Offset=(this->height()-newHeight)/2;
                            float Proporcion=float(m_PictureOriginal.width())/float(this->width());
                            QRect dummyrect=QRect(pulsacion.x1*1000*Proporcion/1000,(pulsacion.y1-Offset)*1000*Proporcion/1000,width*1000*Proporcion/1000,heigth*1000*Proporcion/1000);
                            rect=dummyrect;
                        }
                        Imagen=m_PictureOriginal.copy(rect);
                    }
                    else
                        Imagen=this->pixmap()->copy(rect);

                    if ((1000*this->width()/this->height())>=(1000*Imagen.width()/Imagen.height())){
                        QPixmap dummy=Imagen.scaledToHeight(this->height()/*,Qt::SmoothTransformation*/);
                        int offset_x=(this->width()-dummy.width())/2;
                        int offset_y=(this->height()-dummy.height())/2;
                        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
                        this->setPixmap(pic,true);
                    }
                    else{
                        QPixmap dummy=Imagen.scaledToWidth(this->width()/*,Qt::SmoothTransformation*/);
                        int offset_x=(this->width()-dummy.width())/2;
                        int offset_y=(this->height()-dummy.height())/2;
                        p.drawPixmap(offset_x,offset_y,dummy); // and draw your original pixmap on it
                        this->setPixmap(pic,true);
                    }
                }
            }
        }
}
/** Vamos guardando los zooms*/
void In2QLabel::setPixmap(const QPixmap &p,bool save){    
    if (save){
        QPixmap dummy;
        dummy=this->pixmap()->copy(this->pixmap()->rect());
        prevPixmap.append(dummy);
    }
    QLabel::setPixmap(p);
}

/** Sobrecarg*/
void In2QLabel::setPixmap(const QPixmap &p){
    //setOriginalPixmap(p);
    QLabel::setPixmap(p);
}

void In2QLabel::setOriginalPixmap(const QPixmap &p){
    if (m_PictureOriginal.isNull()||this->prevPixmap.count()==0){
        this->m_PictureOriginal=p;        
    }
    pulsacion.zooming=false;
    QLabel::setPixmap(p);
}
/** Limpia los zooms previos*/
void In2QLabel::setNewPixmap(const QPixmap &p){
    this->prevPixmap.clear();
    setOriginalPixmap(p);
}


void In2QLabel::mouseDoubleClickEvent(QMouseEvent * event){
    m_pressed=false;
    if (this->pixmap()){
        if (event->button()==Qt::LeftButton){
            int x=event->x();
            int y=event->y();
            emit DobleClick(x,y);
        }
    }
}
/** Devuelve el QPixmap origina*/
QPixmap In2QLabel::OriginalPixmap(){
    return this->m_PictureOriginal;
}
/** Devuelve el color del ultimo pixel seleccionado*/
QRgb In2QLabel::SelectedPixelColor(){
    return this->pixmap()->toImage().pixel(pulsacion.x1,pulsacion.y1);
}

/** Devuelve el color del ultimo pixel seleccionado*/
QPoint In2QLabel::GetFirstColorOcurrence(QRgb color){
    QImage dummyImage;
    QPoint dummyPoint;
    //Por defecto punto erroneo
    dummyPoint.setX(-1);
    dummyPoint.setY(-1);
    dummyImage=/*this->pixmap()->toImage();*/this->m_PictureOriginal.toImage();
    int exitFlag=0;
    for (int x=0;x<dummyImage.width();x++){
        if (exitFlag) break;
        for (int y=0;y<dummyImage.height();y++){
            QRgb dummycolor=dummyImage.pixel(x,y);
            if (dummycolor==color){
                //Es la primera ocurrencia
                dummyPoint.setX(x);
                dummyPoint.setY(y);
                exitFlag=1;
                break;
            }
        }
    }
    return dummyPoint;
}

/** Añade un campo de posicion*/
void In2QLabel::setNewMimePixmap(const QPixmap & tmpPixmap){
    if (!this->acceptDrops()){ //Esto es que ya hay otro
        this->SetAcceptDrops(true);
        QLabel *child = new QLabel(this);
        //child=tmpLabel;
        if (!child)
            return;

        child->setGeometry(tmpPixmap.rect());
        child->setPixmap(tmpPixmap);
        child->setFrameShape(QFrame::Box);
        child->setAttribute(Qt::WA_DeleteOnClose);
        child->show();
    }
}


