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
#include "in2random.h"
#include <QTime>
#include <cstdlib>

In2Random::In2Random(QObject *parent) : QObject(parent)
{

}

In2Random::~In2Random()
{

}
/** Genera numero aleatorio*/
int In2Random::generarNumeroAleatorio(int limiteInferior,int limiteSuperior){

    int valorActual;

    int generado = qrand();
    //#define	RAND_MAX	0x7FFF = 32767
    double randMax=RAND_MAX;

    double dGenerado=(double) (generado/randMax);

    int dummygenerado=(int) (dGenerado*(limiteSuperior-limiteInferior));

    valorActual=limiteInferior+dummygenerado;
    if (valorActual<limiteInferior) valorActual=limiteInferior;
    if (valorActual>limiteSuperior) valorActual=limiteSuperior;

    return valorActual;

}
/** Cambia la semilla para generar aleatorios*/
void In2Random::cambiarSemillaNumeroAleatorio(void){
    int semilla=QTime::currentTime().toString("zzz").toInt();
    qsrand(semilla);
}
