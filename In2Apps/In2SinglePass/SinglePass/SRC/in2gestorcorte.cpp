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
#include "in2gestorcorte.h"
#include "in2configuratorcorte.h"


In2GestorCorte *In2GestorCorte::mInstance  = NULL;

/** Singleton*/
In2GestorCorte *In2GestorCorte::request(QObject *parent){
    if(mInstance == NULL){
        mInstance = new In2GestorCorte(parent);
    }
    return mInstance;
}

/** Devuelve si esta habilitado*/
bool In2GestorCorte::isEnabled(){
    return m_enabled;
}

/** Gestor de corte*/
In2GestorCorte::In2GestorCorte(QObject *parent) : QObject(parent){
    In2ConfiguratorCorte config;
    m_enabled=config.isEnabled();
}

/** Activa las cuchillas*/
void In2GestorCorte::sltActivate(){
    if (m_enabled)
        emit sgnNewDataCorte(ITEM_BAJAR_CUCHILLAS,1);
}
/** Desactiva las cuchillas*/
void In2GestorCorte::sltDeActivate(){
    if (m_enabled)
        emit sgnNewDataCorte(ITEM_BAJAR_CUCHILLAS,0);
}
/** Devuelve si está activo*/
bool In2GestorCorte::isActive(){
    bool val=false;
    if (m_enabled){
        ClienteOPC *p=ClienteOPC::request(0);
        if (p){
            val=p->LeerDato(ITEM_BAJAR_CUCHILLAS).toBool();
        }
    }
    return val;
}

/** Devuelve la fuerza de cuchillas*/
int In2GestorCorte::getFuerza(){
    int val=0;
    if (m_enabled){
        ClienteOPC *p=ClienteOPC::request(0);
        if (p){
            val=p->LeerDato(ITEM_FUERZA_CUCHILLAS).toInt();
        }
    }
    return val;
}
/** Devuelve la fuerza de cuchillas*/
void In2GestorCorte::setFuerza(int val){
    if (m_enabled)
        emit sgnNewDataCorte(ITEM_FUERZA_CUCHILLAS,val);
}

/** Invierte el valor de corte y devuelve lo que pone*/
bool In2GestorCorte::sltToogleCorte(){
    bool newVal=false;
    if (m_enabled){
        bool prevVal=isActive();
        if (prevVal){
            newVal=false;
            sltDeActivate();
        }
        else{
            newVal=true;
            sltActivate();
        }
    }
    return newVal;
}
