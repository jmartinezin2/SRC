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
#include "in2char.h"

In2Char::In2Char(QObject *parent) :
    QObject(parent)
{
}
/** Devuelve el char en hexadecimal correspondiente al ascii*/
char In2Char::fromAscii(char x){
    if ((x>=0x30)&&(x<=0x39))return x-0x30;
    else if ((x>=0x41)&&(x<=0x46)) return x-0x37;
    else return 0;
}
char In2Char::fromHex(char x){
    if (x==0) return '0';
    else if (x==1) return '1';
    else if (x==2) return '2';
    else if (x==3) return '3';
    else if (x==4) return '4';
    else if (x==5) return '5';
    else if (x==6) return '6';
    else if (x==7) return '7';
    else if (x==8) return '8';
    else if (x==9) return '9';
    else if (x==10) return 'A';
    else if (x==11) return 'B';
    else if (x==12) return 'C';
    else if (x==13) return 'D';
    else if (x==14) return 'E';
    else if (x==15) return 'F';
    else return '0';
}

