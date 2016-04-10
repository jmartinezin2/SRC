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
#include "in2graphicsscene.h"
#include <QVarLengthArray>
#include <QPainter>
#include "in2defines.h"
#include "in2fieldpixmapitem.h"

//Dimensioones de ancho de grid*/
#define DEFAULT_GRID_SIZE 5
#define MAX_GRID_SIZE 25
#define MIN_GRID_SIZE 0.1





In2GraphicsScene::In2GraphicsScene(qreal x, qreal y, qreal w, qreal h)
    : QGraphicsScene(x, y, w, h){

    m_benableGrid=false;
    m_gridSize=DEFAULT_GRID_SIZE;
}
In2GraphicsScene::~In2GraphicsScene(){

}

/** Devuelve si el grid está habilitado*/
bool In2GraphicsScene::isGridEnabled(){
    return m_benableGrid;
}

void In2GraphicsScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    if (m_benableGrid){

        const float gridSize = FACTOR_PIXELS_MM*m_gridSize;
        QRectF fooRect=this->sceneRect();
        qreal left=fooRect.left();
        qreal top=fooRect.top();
        //Primero se cuentan las lineas

        lines.clear();
        int num=0;
        painter->save();
        QFont font;
        int pointSize=gridSize/3;
        if (pointSize==0) pointSize=1;
        font.setPointSize(pointSize);
        painter->setFont(font);
        for (qreal x = left; x < fooRect.right(); x += gridSize){
            lines.append(QLineF(x, fooRect.top(), x, fooRect.bottom()));
            painter->drawText(x,fooRect.top()+gridSize/2,QString("%1").arg(num));
            num+=m_gridSize;
            //countH++;
        }
        //int countV=0;
        num=0;
        for (qreal y = top; y < fooRect.bottom(); y += gridSize){
            lines.append(QLineF(fooRect.left(), y, fooRect.right(), y));
            painter->drawText(fooRect.left(),y,QString("%1").arg(num));
            num+=m_gridSize;
        }
        //painter->save();
        QPen pen(Qt::gray, 1, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
        pen.setCosmetic(true);
        painter->setPen( pen );
        painter->drawLines(lines.data(), lines.size());
        painter->restore();
    }
}


/** Devuelve la posición de las líneas*/
QVector<QLineF> In2GraphicsScene::getGridLines(){
    return lines;
}

/** Carga un nuevo grid*/
void In2GraphicsScene::setGridSize(float size){
    if ((size<=MAX_GRID_SIZE)&&(size>=MIN_GRID_SIZE)){
        m_gridSize=size;
        this->update(this->sceneRect());
    }
}
/** Devuelve el tamaño de grid*/
float In2GraphicsScene::getGridSize(){
    return m_gridSize;
}

/** Activa/desactiva el grid*/
void In2GraphicsScene::enableGrid(bool value){
    m_benableGrid=value;
    this->update();


}

/** Devuelve el item que está seleccionado*/
In2FieldPixmapItem* In2GraphicsScene::getFieldSelected(){

    In2FieldPixmapItem* sel=NULL;
    QList<QGraphicsItem*> items=this->items();
    for (int x=0;x<items.count();x++){
        In2FieldPixmapItem *p=static_cast<In2FieldPixmapItem*>(items.at(x));
        if (p->isSelected()) {
            sel=p;
            break;
        }
    }
    return sel;
}

/** Devuelve el item que está seleccionado*/
In2FieldPixmapItem* In2GraphicsScene::getFieldSelectedFromID(int id){

    In2FieldPixmapItem* sel=NULL;
    QList<QGraphicsItem*> items=this->items();
    for (int x=0;x<items.count();x++){
        In2FieldPixmapItem *p=static_cast<In2FieldPixmapItem*>(items.at(x));
        if (p->getID()==id) {
            sel=p;
            break;
        }
    }
    return sel;
}



/** Devuelve el item que está seleccionado*/
In2GraphicsPixmapItem* In2GraphicsScene::getItemSelected(){

    In2GraphicsPixmapItem* sel=NULL;
    QList<QGraphicsItem*> items=this->items();
    for (int x=0;x<items.count();x++){
        In2GraphicsPixmapItem *p=static_cast<In2GraphicsPixmapItem*>(items.at(x));
        if (p->isSelected()) {
            sel=p;
            break;
        }
    }
    return sel;
}

