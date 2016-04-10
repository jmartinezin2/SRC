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

#include "in2font.h"
#include <QFont>

In2Font::In2Font(QObject *parent) : QObject(parent)
{
    m_EstiloFuente=(QStringList()<<"Light"<<"Normal"<<"DemiBold"<<"Bold"<<"Black");
    m_EstiloFuenteQt=(QList<int>()<<QFont::Light<<QFont::Normal<<QFont::DemiBold<<QFont::Bold<<QFont::Black);

}

In2Font::~In2Font()
{

}
/** Devuelve la lista de fuentes*/
QStringList In2Font::getFontStyleList(){
    return m_EstiloFuente;
}
/** Devuelve la lista de pesos*/
QList<int> In2Font::getQtFontStyleList(){
    return m_EstiloFuenteQt;
}
/** Devuele un peso especifico*/
int In2Font::getQtFontWeight(int index){
    int rtn=m_EstiloFuenteQt.at(0);
    if (index<m_EstiloFuenteQt.count()) rtn=m_EstiloFuenteQt.at(index);
    return rtn;
}
/** Devuele un peso especifico*/
int In2Font::getQtFontWeight(QString name){
    int rtn=0;
    int index=getFontStyleIndex(name);
    if (index!=-1) rtn=getQtFontWeight(index);
    return rtn;
}


/** Devuelve el indice de la fuente*/
int In2Font::getFontStyleIndex(QString font){
    int index=m_EstiloFuente.indexOf(font);
    return index;
}

