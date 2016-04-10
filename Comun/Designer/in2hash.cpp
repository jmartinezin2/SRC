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

#include "in2hash.h"
#include <QHash>
#include <QCryptographicHash>
#include <QFile>

In2Hash::In2Hash(QObject *parent) : QObject(parent)
{

}

In2Hash::~In2Hash()
{

}
/** Calcula un hash*/
QByteArray In2Hash::CalculateHash(QByteArray DatosEncriptar){
    QByteArray resultado;
    QCryptographicHash MyHash(QCryptographicHash::Sha1);
    MyHash.addData(DatosEncriptar);
    resultado=MyHash.result().toHex();
    return resultado;
}

/** Testea un hash*/
bool In2Hash::TestHash(QByteArray DatosEncriptar,QByteArray ClaveEncriptada){

    bool bRet;

    QByteArray DatosEncriptados=CalculateHash(DatosEncriptar);

    if (ClaveEncriptada==DatosEncriptados) bRet=true;
    else bRet=false;

    return bRet;

}

/** Devuelve si el hash es correcto*/
bool In2Hash::HashOk(QByteArray HashLeido,QByteArray DatosEncriptar){
    bool res;
    QByteArray resultado;
    resultado=CalculateHash(DatosEncriptar);
    if (resultado==HashLeido) res=true;
    else res=false;
    return res;
}


/** Genera hash que identifica al fichero...lo hace sobre todos sus datos + fecha de creacion + fecha ultima modificacion*/
QByteArray In2Hash::GenerateHashFromFile(const QString &strFile){
    QByteArray output;
    QFile file(strFile);
    if (file.exists(strFile)){
        file.open(QIODevice::ReadOnly);
        QByteArray byteFile=file.readAll();
        file.close();
        output=CalculateHash(byteFile);
    }
    return output;
}
