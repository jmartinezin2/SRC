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
#include "in2gramarerp.h"
#include <QFile>
#include <QTextStream>
#include "in2configuratorerp.h"


In2GramarErp::In2GramarErp(QObject *parent) :
    QObject(parent)
{
}
/** Se chequea el fichero*/
QStringList In2GramarErp::checkERP(){
    QStringList data;
    In2ConfiguratorERP config;
    config.readConfig();
    QString file=config.getInterFile();
    QFile foo(file);
    if (foo.exists()){
        if (foo.open(QIODevice::ReadOnly)){
            QTextStream stream(&foo);
            while (!stream.atEnd())
            {
                data.append(stream.readLine());
            }
        }
    }
    return data;
}
