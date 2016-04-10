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
#include "in2magicpacket.h"
#include <QStringList>
#include <QUdpSocket>

In2MagicPacket::In2MagicPacket(QObject *parent) :
    QObject(parent)
{
}
/** Manda un Magic Packet a la MAC*/
void In2MagicPacket::SendPacket(QString strMAC){
    QByteArray packet;
    packet.fill(0xFF,6);
    QStringList lst=strMAC.split(":");
    QByteArray bArrayMac;
    bool ok;
    for (int x=0;x<lst.count();x++){
        const unsigned int foo=lst.at(x).toInt(&ok,16);
        bArrayMac.append(foo);
    }
    //Metemos la direccion 16 veces
    for (int x=0;x<16;x++){
        packet.append(bArrayMac);
    }

    QUdpSocket p;
    QHostAddress myBroadcastAddress = QHostAddress("255.255.255.255");
    p.writeDatagram(packet,myBroadcastAddress,9);
}
