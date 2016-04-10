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
#include "in2opcserver.h"
#include "clienteopc.h"
In2OPCServer::In2OPCServer(QObject *parent) :
    QThread(parent)
{
}

void In2OPCServer::run(){
    /*ClienteOPC *opc=ClienteOPC::request(0);
    while (1){
        DWORD diametro_bobina_salida=opc->LeerDato(ITEM_DIAMETRO_SALIDA_ACT).toInt();
        DWORD velocidad=opc->LeerDato(ITEM_VELOCIDAD_ACT).toInt(); //velocidad actual en 100*m/min
        DWORD lampara_on=opc->LeerDato(ITEM_LAMPARA_ON).toBool();
        DWORD intensidad_lampara=opc->LeerDato(ITEM_INTENSIDAD_LAMPARA).toInt();
        DWORD diametro_bobina_entrada_inst=opc->LeerDato(ITEM_DIAMETRO_ENTRADA_ACT).toInt();
        DWORD foo=opc->LeerDato(ITEM_MAX_DISTANCIA_MACULA).toInt();
    }*/

}
