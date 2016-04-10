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
#include "in2fieldpixmapitem.h"

In2FieldPixmapItem::In2FieldPixmapItem(QGraphicsPixmapItem *parent,int id) :
    QGraphicsPixmapItem(parent)
{
    m_selected=false;
    m_id=id;
}

In2FieldPixmapItem::~In2FieldPixmapItem()
{

}
/** Devuelve el ID*/
int In2FieldPixmapItem::getID(){
    return m_id;
}

/** Devuelve si esta seleccionado*/
bool In2FieldPixmapItem::isSelected(){
    return m_selected;
}

/** Cuando se deja de presionar el mouse*/
void In2FieldPixmapItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ){
    m_selected=false;
    emit sgnFieldReleased(m_id);
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

/** Cuando se presiona el mouse*/
void In2FieldPixmapItem::mousePressEvent ( QGraphicsSceneMouseEvent * event ){
    m_selected=true;
    emit sgnFieldPressed(m_id);
    QGraphicsPixmapItem::mousePressEvent(event);

}
