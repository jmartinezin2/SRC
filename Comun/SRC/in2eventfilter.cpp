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
#include "in2eventfilter.h"

//15 minutos
#define SCREEN_TIME 60000 //900000


In2EventFilter *In2EventFilter::mInstance  = NULL;


/** Singleton*/
In2EventFilter *In2EventFilter::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new In2EventFilter(parent);
    }
    return mInstance;

}

/** resetea el filtro de eventos*/
void In2EventFilter::reset(){
    m_timer.start();
}

In2EventFilter::In2EventFilter(QObject *parent) :
    QObject(parent)
{
    connect(&m_timer,SIGNAL(timeout()),this,SIGNAL(timeout()));
    m_timer.start(SCREEN_TIME);
}
/** Filtro de eventos*/
bool In2EventFilter::eventFilter(QObject* object,QEvent* event)
{
    if(event->type() == QEvent::KeyPress){
        m_timer.start();
    }
    else if (event->type()==QEvent::MouseMove){
        m_timer.start();
    }
    return QObject::eventFilter(object,event);
};


