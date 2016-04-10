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
#include "counterStyle.h"
#include "MemoryMap.h"
#include "globals.h"

#include <windows.h>
#include <winldap.h>
#include <winber.h>
#include <rpc.h>
#include <rpcdce.h>
#include <stdio.h>

#include <QString>
#include <QLocale>


counterStyle::counterStyle(QObject *parent) :
    QObject(parent)
{
    localePrevio=QLocale::system();
    miLocale = QLocale::c();

}


counterStyle::~counterStyle(){

	QLocale::setDefault(localePrevio);

}

/** Da formato al campo..utilizamos Macau como si no quisieramos dar formato*/
QString counterStyle::darformato(quint64 value, struct FormatoNumero & f){

    QString ret;
    QString s;

    //miLocale=QLocale(loc);
    miLocale = QLocale(f.FormatSeparador,QLocale::AnyCountry);
    QLocale::setDefault(miLocale);
    if (f.bFormatEnabled)
        s=QString("%L1").arg(value);
    else
       s=QString("%1").arg(value);
    if (f.zeroPadding)
        ret=s.rightJustified(f.numeroCifras,'0');
    else ret=s;
    QLocale::setDefault(localePrevio);

    return ret;
}
