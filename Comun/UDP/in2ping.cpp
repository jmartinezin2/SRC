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
#include "in2ping.h"
#include <QProcess>
#include <QTextStream>



In2Ping::In2Ping(QObject *parent) :
    QObject(parent)
{
}
/** Ejecuta un ping*/
bool In2Ping::ping(QString host){
    bool rtn=false;
    QProcess foo;
    foo.start("cmd.exe",QStringList()<<"/c"<<"ping"<<"-n"<<"1"<<host);
    if (foo.waitForStarted()){
        if (foo.waitForFinished()){
            if (foo.exitCode()==0){
                QByteArray data=foo.readAllStandardOutput();
                if (data.contains("TTL=")) rtn=true;

            }
        }
     }

    return rtn;
}
