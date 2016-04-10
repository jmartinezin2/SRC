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
#include "imgvargenerator.h"
#include "in2defines.h"
#include <QtGlobal>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QImage>
#include <QThread>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QVector>
#include <QRgb>
#include <QColor>
#include "In2BarcodeClass.h"
#include "counterStyle.h"
#include "in2random.h"
#include "in2font.h"


/** Crea el objeto para 1 tarjeta...le pasamos la referencia del Qstring*/
ImgVarGenerator::ImgVarGenerator(QObject *parent) :
    QObject(parent)
{
    memoryMap=MemoryMap::request(this);
    m_barcode=new In2BarcodeClass();
    Reset();
}

void ImgVarGenerator::Reset(){

    Xo=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.PuntoInicialX);
    Yo=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.PuntoInicialY);
    sx=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.SeparacionX);
    sy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_variables.SeparacionY);

    table.clear();
    for( int j = 0; j < 256; ++j ){
        table.append(qRgb(j,j,j));
    }

    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
            if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                //La variable es de tipo contador y hay que sacar su valor conforme a valorinicial, incremento...
                limite_inferior[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.limiteInferior;
                limite_superior[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.limiteSuperior;
                valor_inicial[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial;
                incremento[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.incremento;
                offset_columna[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;

                int impresionesTotalesPorFilas=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas
                        *memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas
                        *memoryMap->m_mapa.m_camposvariables[i].m_matrices[0].FilasMatrizCampo
                        *memoryMap->m_mapa.m_camposvariables[i].m_matrices[0].ColumnasMatrizCampo;

                int impresionesTotalesPorColumnas=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas
                        *memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas
                        *memoryMap->m_mapa.m_camposvariables[i].m_matrices[0].FilasMatrizCampo
                        *memoryMap->m_mapa.m_camposvariables[i].m_matrices[0].ColumnasMatrizCampo;



                if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
                    if (impresionesTotalesPorFilas==0) impresionesTotalesPorFilas=1;
                    //hay que incluir Impresiones por Envio
                    if (incremento[i]>0){
                        modulo[i]=(long) ((limite_superior[i]-limite_inferior[i])/(incremento[i]*impresionesTotalesPorFilas));
                        modulo_inicial[i]=(long) ((limite_superior[i]-valor_inicial[i])/(incremento[i]*impresionesTotalesPorFilas));
                    }
                    else{
                        modulo[i]=(long) ((limite_superior[i]-limite_inferior[i])/(incremento[i]*impresionesTotalesPorFilas));
                        modulo_inicial[i]=(long) ((valor_inicial[i]-limite_inferior[i])/(incremento[i]*impresionesTotalesPorFilas));
                    }
                }
                else{ //if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
                    //hay que incluir filasMatriz*filasComposicion
                    if (impresionesTotalesPorColumnas==0) impresionesTotalesPorColumnas=1;
                    if (incremento[i]>0){
                        modulo[i]=(long) ((limite_superior[i]-limite_inferior[i])/(incremento[i]*impresionesTotalesPorColumnas));
                        modulo_inicial[i]=(long) ((limite_superior[i]-valor_inicial[i])/(incremento[i]*impresionesTotalesPorColumnas));
                    }
                    else{
                        modulo[i]=(long) ((limite_superior[i]-limite_inferior[i])/(incremento[i]*impresionesTotalesPorColumnas));
                        modulo_inicial[i]=(long) ((valor_inicial[i]-limite_inferior[i])/(incremento[i]*impresionesTotalesPorColumnas));
                    }
                }

                //qDebug() << "Modulos: " << modulo[i] << modulo_inicial[i];
            }

        }
    }
}


/** Devuelve la configuracion de la fuente*/
QFont ImgVarGenerator::configureFont(int i){
    QFont mifont;
    unsigned int tamano_fuente_campo;
    tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
    In2Font in2Font;
    int peso_campo=in2Font.getQtFontWeight(memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo);
    mifont.setPixelSize(tamano_fuente_campo);
    mifont.setStyleStrategy(QFont::PreferAntialias);
    mifont.setWeight(peso_campo);
    mifont.setFamily(memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo);
    return mifont;
}

//Crea cada una de las imagenes que forma la impresion
QImage ImgVarGenerator::CreaImagenVariablePorColumnas(int index,QImage::Format format){


    unsigned int Xtemp,Ytemp;
    unsigned int Xotemp,Yotemp;
    unsigned int dix,diy;

    unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
    QString Vtemp;
    long dummy_index;

    //int index1,index2;

    QPixmap PixmapPlantilla(memoryMap->m_Imagen.width(),memoryMap->m_Imagen.height());
    QPixmap pic(PixmapPlantilla.size());
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::gray);
    p.drawPixmap(0,0,memoryMap->m_Imagen); // and draw your original pixmap on it
    //Esto ralentiza
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

    QStringList lista_excepciones;
    unsigned char indice_color=0;
    long valor_actual=0;
    //long valor_inicial_fila=0;

    filasMatriz=0;
    columnasMatriz=0;
    filasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
    columnasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    impresionesPorPagina=0;

    indicePagina=index;
    valorInicialPagina=0;

    //int numCamposVariablesDB=miDataBaseCampos.CamposDatabase;

    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
            //tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
            offset_columna[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;
            QFont mifont=configureFont(i);
            p.setFont(mifont);
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                p.setPen(Qt::black);
            }
            else{
                indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
            }
            dummy_index=(index-1)%limite_index[i];
            int numMatrices=memoryMap->getNumMatricesCampoVariable(i);

            //valor_inicial_fila=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila;//+incremento*(index-1);

            for (int j=0;j<numMatrices/*NUM_MATRIZ_POR_CAMPO*/;j++){
                if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                    lista_excepciones.clear();
                    excepciones=QString(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);
                    Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                    dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);

                    for (unsigned int filaComposicion=0;filaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;filaComposicion++){
                        //valor_actual=valor_inicial_fila;
                        for (unsigned int columnaComposicion=0;columnaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;columnaComposicion++){
                            Ytemp=Yo+Yotemp+filaComposicion*sy;
                            celda=1;
                            for (unsigned int filaMatriz=0;filaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                                Xtemp=Xo+Xotemp+columnaComposicion*sx;
                                for (unsigned int columnaMatriz=0;columnaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                    celda_s=QString("%1").arg(celda);
                                    esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                    if (esunaexcepcion==false){
                                        p.save();
                                        //Si es un campo de tipo CONTADOR
                                        if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){                    
                                            if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
                                                //Impresion por columnas
                                                impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;

                                                valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                        +incremento[i]*(indicePagina)*filasMatriz*filasComposicion;
                                                valor_actual= valorInicialPagina+(filaMatriz+(filaComposicion)*filasMatriz)*incremento[i]+offset_columna[i]*(columnaMatriz+columnasMatriz*columnaComposicion);
                                                /*int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                                Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);*/
                                                counterStyle numEstilo;
                                                Vtemp=numEstilo.darformato(valor_actual,memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format);

                                                //qDebug() << "valor_actual contador: " << valor_actual;
                                            }
                                            else if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
                                                //Impresión por filas
                                                impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;
                                                valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                        +incremento[i]*(indicePagina)*impresionesPorPagina;
                                                valor_actual=valorInicialPagina+((columnasMatriz*columnasComposicion*(filaMatriz)+columnaMatriz+columnasMatriz*(columnaComposicion)+(filaComposicion)*columnasComposicion*columnasMatriz*filasMatriz))*incremento[i];
                                                /*int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                                Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);*/
                                                counterStyle numEstilo;
                                                Vtemp=numEstilo.darformato(valor_actual,memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format);


                                            }
                                            dummy_index++;

                                        }
                                        //Si es un campo de origen BASE DE DATOS
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                            //A revisar
                                            /*int LocalOffset=index*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas*numCamposVariablesDB;
                                            int LocalIndex=i+(filaComposicion*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas+columnaComposicion)*numCamposVariablesDB;
                                            int guardIndex=LocalOffset+LocalIndex;
                                            if (guardIndex<miDataBaseCampos.m_Tabla.count())
                                                Vtemp=miDataBaseCampos.m_Tabla[LocalOffset+LocalIndex];
                                            else Vtemp="";*/

                                        }
                                        //Si es un campo de tipo Fecha/Hora
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){                                 
                                            QDateTime fechahora;
                                            QString sFechaHora;
                                            sFechaHora=fechahora.currentDateTime().toString(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato));
                                            Vtemp=QString("%1").arg(sFechaHora);
                                        }
                                        //Si es un campo de tipo Aleatorio
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                            int limiteInferior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior;
                                            int limiteSuperior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior;
                                            int numeroCifras=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras;
                                            In2Random In2Rand;
                                            int valorAleatorioActual=In2Rand.generarNumeroAleatorio(limiteInferior,limiteSuperior);
                                            Vtemp=QString("%1").arg(valorAleatorioActual).rightJustified(numeroCifras,'0',true);
                                        }
                                        else{
                                            Vtemp=QString("Origen?");
                                        }

                                        if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                            //Retrocedemos
                                            //Retrocedemos
                                            //p.translate(0,(-1)*tamano_fuente_campo);
                                            p.translate(Xtemp,Ytemp);

                                            //p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                            struct m_BarcodeParameters  barcodeParameters=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                            m_barcode->setParams(barcodeParameters);
                                            m_barcode->SetPixmap(Vtemp);
                                            int Orientacion=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            QPixmap foo=m_barcode->qpxImagen.transformed(QTransform().rotate(Orientacion));
                                            p.drawPixmap(0,0,foo);

                                            //p.drawPixmap(0,0,m_barcode->qpxImagen);

                                        }
                                        else {
                                            int Orientacion=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            p.translate(Xtemp,Ytemp/*+tamano_fuente_campo*/);
                                            QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                            QPixmap dest;
                                            if (Orientacion){
                                                dest=foo.transformed(QTransform().rotate(Orientacion));
                                                p.drawPixmap(0,0,dest);
                                                //this->drawHoverRect(p,dest.rect());
                                            }
                                            else{
                                                p.drawPixmap(0,0,foo);
                                                //this->drawHoverRect(p,foo.rect());
                                            }

                                        }
                                        p.restore();
                                    }
                                    Xtemp+=dix;
                                    celda++;
                                }
                                Ytemp+=diy;
                            }
                        }//columna
                        //valor_inicial_fila+=incremento[i];
                        //memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila=valor_inicial_fila;
                    }//filaComposicion


                } //if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
            }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        } //if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
    } //for (i=0;i<NUM_CAMPOS;i++){

    p.end();

    PixmapPlantilla=pic.transformed(QTransform().rotate(0,Qt::ZAxis));
    //PixmapPlantilla.save(QString("test%1.bmp").arg(index),"BMP");
    if (format==QImage::Format_Mono)
        return PixmapPlantilla.toImage().convertToFormat(QImage::Format_Mono,Qt::MonoOnly|Qt::ThresholdDither);
    else //De momento lo forzamos a indexado a 8 bit
        return PixmapPlantilla.toImage().convertToFormat(QImage::Format_Indexed8,table,Qt::AutoColor);


}


//Crea cada una de las imagenes que forma la impresion
QImage ImgVarGenerator::CreaImagenVariablePorColumnasUpdated(int index,QImage::Format format){

    //unsigned char tamano_fuente_campo;
    unsigned int Xtemp,Ytemp;
    unsigned int Xotemp,Yotemp;
    unsigned int dix,diy;

    unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
    QString Vtemp;


    //int index1,index2;
    filasMatriz=1;
    columnasMatriz=1;

    filasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
    columnasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;

    QImage PixmapPlantilla(memoryMap->m_Imagen.width(),memoryMap->m_Imagen.height(),QImage::Format_RGB32);
	QPainter p;   // create a QPainter for it
    p.begin(&PixmapPlantilla);
    p.drawPixmap(0,0,memoryMap->m_Imagen); // and draw your original pixmap on it
    //Esto ralentiza
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);


    QStringList lista_excepciones;
    unsigned char indice_color=0;
    valor_actual=0;
    //long valor_inicial_fila=0;
    valorInicialPagina=0;

    //int numCamposVariablesDB=miDataBaseCampos.CamposDatabase;

    indicePagina=index;
    impresionesPorPagina=0;

    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
            //tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
            offset_columna[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;
            QFont mifont=configureFont(i);
            p.setFont(mifont);
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                p.setPen(Qt::black);
                //p.setPen(0);
            }
            else{
                indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);

            }
            //dummy_index=(index-1)%limite_index[i];
            int numMatrices=memoryMap->getNumMatricesCampoVariable(i);

            for (int j=0;j<numMatrices/*NUM_MATRIZ_POR_CAMPO*/;j++){
                if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                    lista_excepciones.clear();
                    excepciones=QString(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);
                    Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                    dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);
                    filasMatriz=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;
                    columnasMatriz=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;

                    for (unsigned int filaComposicion=0;filaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;filaComposicion++){
                        for (unsigned int columnaComposicion=0;columnaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;columnaComposicion++){
                            Ytemp=Yo+Yotemp+filaComposicion*sy;
                            celda=1;
                            for (unsigned int filaMatriz=0;filaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                                Xtemp=Xo+Xotemp+columnaComposicion*sx;
                                for (unsigned int columnaMatriz=0;columnaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                    celda_s=QString("%1").arg(celda);
                                    esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                    if (esunaexcepcion==false){
                                        p.save();

                                        //Si es un campo de tipo CONTADOR
                                        valorActualOutOfLimits=false;
                                        if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                            if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
                                                //Impresion por columnas
                                                indicePagina=calculoNuevoIndex(i);
                                                impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;
                                                valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                        +incremento[i]*(indicePagina)*filasMatriz*filasComposicion;
                                                controlContadorValorInicialPagina(i);
                                                valor_actual= valorInicialPagina+(filaMatriz+(filaComposicion)*filasMatriz)*incremento[i]+offset_columna[i]*(columnaMatriz+columnasMatriz*columnaComposicion);
                                                /*int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                                Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);*/
                                                counterStyle numEstilo;
                                                Vtemp=numEstilo.darformato(valor_actual,memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format);


                                            }
                                            else if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
                                                //Impresión por filas
                                                indicePagina=calculoNuevoIndex(i);
                                                impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;
                                                valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                        +incremento[i]*(indicePagina)*impresionesPorPagina;
                                                controlContadorValorInicialPagina(i);
                                                valor_actual=valorInicialPagina+((columnasMatriz*columnasComposicion*(filaMatriz)+columnaMatriz+columnasMatriz*(columnaComposicion)+(filaComposicion)*columnasComposicion*columnasMatriz*filasMatriz))*incremento[i];
                                                /*int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                                Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);*/
                                                counterStyle numEstilo;
                                                Vtemp=numEstilo.darformato(valor_actual,memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format);


                                                qDebug() << "valor_actual barcode: " << valor_actual;
                                            }
                                            valorActualFueraDeLimites(i);
                                        }
                                        //Si es un campo de origen BASE DE DATOS
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                            //A revisar
                                           /* int LocalOffset=index*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas*numCamposVariablesDB;
                                            int LocalIndex=i+(filaComposicion*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas+columnaComposicion)*numCamposVariablesDB;
                                            //Vtemp=miDataBaseCampos.m_Tabla[index*numCamposVariables*numMatrices+i];
                                            Vtemp=miDataBaseCampos.m_Tabla[LocalOffset+LocalIndex];*/

                                        }
                                        //Si es un campo de tipo Fecha/Hora
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                            QDateTime fechahora;
                                            QString sFechaHora;
                                            sFechaHora=fechahora.currentDateTime().toString(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato));
                                            Vtemp=QString("%1").arg(sFechaHora);
                                        }
                                        //Si es un campo de tipo Aleatorio
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                            int limiteInferior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior;
                                            int limiteSuperior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior;
                                            int numeroCifras=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras;
                                            In2Random In2Rand;
                                            int valorAleatorioActual=In2Rand.generarNumeroAleatorio(limiteInferior,limiteSuperior);
                                            Vtemp=QString("%1").arg(valorAleatorioActual).rightJustified(numeroCifras,'0',true);
                                        }
                                        else{
                                            Vtemp=QString("Origen?");
                                        }
                                        if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                            p.translate(Xtemp,Ytemp);
                                            //p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);

                                            struct m_BarcodeParameters  barcodeParameters=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                            m_barcode->setParams(barcodeParameters);
                                            m_barcode->SetPixmap(Vtemp);

                                            int Orientacion=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            QPixmap foo=m_barcode->qpxImagen.transformed(QTransform().rotate(Orientacion));
                                            p.drawPixmap(0,0,foo);

                                            //p.drawPixmap(0,0,m_barcode->qpxImagen);
                                            //delete barcode;
                                        }
                                        else {
                                            int Orientacion=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            p.translate(Xtemp,Ytemp/*+tamano_fuente_campo*/);
                                            QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                            QPixmap dest;
                                            if (Orientacion){
                                                dest=foo.transformed(QTransform().rotate(Orientacion));
                                                p.drawPixmap(0,0,dest);
                                                //this->drawHoverRect(p,dest.rect());
                                            }
                                            else{
                                                p.drawPixmap(0,0,foo);
                                                //this->drawHoverRect(p,foo.rect());
                                            }
                                        }
                                        //Guarda el valor actual
                                        memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo=Vtemp;
                                        p.restore();
                                    }
                                    Xtemp+=dix;
                                    celda++;
                                }
                                Ytemp+=diy;
                            }
                        }//columnaComposicion
                        //valor_inicial_fila+=incremento[i];
                        //memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila=valor_inicial_fila;
                    }//fila


                } //if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
            }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        } //if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
    } //for (i=0;i<NUM_CAMPOS;i++){

    p.end();



    //PixmapPlantilla.save(QString("test%1.bmp").arg(index),"BMP");
    if (format==QImage::Format_Mono)
        return PixmapPlantilla.convertToFormat(QImage::Format_Mono,Qt::MonoOnly);
    else //De momento lo forzamos a indexado a 8 bit
        return PixmapPlantilla.convertToFormat(QImage::Format_Indexed8,table,Qt::AvoidDither);
    /*PixmapPlantilla=pic.transformed(QTransform().rotate(0,Qt::ZAxis));
    //PixmapPlantilla.save(QString("test%1.bmp").arg(index),"BMP");
    return PixmapPlantilla.toImage().convertToFormat(QImage::Format_Indexed8,table,Qt::AutoColor);*/

}

//Crea cada una de las imagenes que forma la impresion
QImage ImgVarGenerator::CreaImagenVariablePorColumnasUpdated(int index,QImage::Format format,bool bHighSpeed){

    //unsigned char tamano_fuente_campo;
    unsigned int Xtemp,Ytemp;
    unsigned int Xotemp,Yotemp;
    unsigned int dix,diy;

    unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
    QString Vtemp;


    //int index1,index2;
    filasMatriz=1;
    columnasMatriz=1;

    filasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
    columnasComposicion=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;

    QImage PixmapPlantilla(memoryMap->m_Imagen.width(),memoryMap->m_Imagen.height(),QImage::Format_RGB32);
    QPainter p;   // create a QPainter for it
    p.begin(&PixmapPlantilla);
    p.drawPixmap(0,0,memoryMap->m_Imagen); // and draw your original pixmap on it
    //Esto ralentiza
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);


    QStringList lista_excepciones;
    unsigned char indice_color=0;
    valor_actual=0;
    //long valor_inicial_fila=0;
    valorInicialPagina=0;

    //int numCamposVariablesDB=miDataBaseCampos.CamposDatabase;

    indicePagina=index;
    impresionesPorPagina=0;

    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
            //tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
            offset_columna[i]=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;
            QFont mifont=configureFont(i);
            p.setFont(mifont);
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                p.setPen(Qt::black);
                indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                //p.setPen(0);
            }
            else{
                indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);

            }
            for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                    lista_excepciones.clear();
                    excepciones=QString(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);
                    Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                    dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);
                    filasMatriz=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;
                    columnasMatriz=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;
                    for (unsigned int filaComposicion=0;filaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;filaComposicion++){
                        for (unsigned int columnaComposicion=0;columnaComposicion<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;columnaComposicion++){
                            Ytemp=Yo+Yotemp+filaComposicion*sy;
                            celda=1;
                            for (unsigned int filaMatriz=0;filaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                                Xtemp=Xo+Xotemp+columnaComposicion*sx;
                                for (unsigned int columnaMatriz=0;columnaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                    celda_s=QString("%1").arg(celda);
                                    esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                    if (esunaexcepcion==false){
                                        p.save();

                                        //Si es un campo de tipo CONTADOR
                                        valorActualOutOfLimits=false;
                                        if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                            if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
                                                //Impresion por columnas
                                                indicePagina=calculoNuevoIndex(i);
                                                impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;
                                                valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                        +incremento[i]*(indicePagina)*filasMatriz*filasComposicion;
                                                controlContadorValorInicialPagina(i);
                                                valor_actual= valorInicialPagina+(filaMatriz+(filaComposicion)*filasMatriz)*incremento[i]+offset_columna[i]*(columnaMatriz+columnasMatriz*columnaComposicion);
                                                counterStyle numEstilo;
                                                Vtemp=numEstilo.darformato(valor_actual,memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format);


                                            }
                                            else if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
                                                //Impresión por filas
                                                indicePagina=calculoNuevoIndex(i);
                                                impresionesPorPagina=filasMatriz*columnasMatriz*filasComposicion*columnasComposicion;
                                                valorInicialPagina=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial
                                                        +incremento[i]*(indicePagina)*impresionesPorPagina;
                                                controlContadorValorInicialPagina(i);
                                                valor_actual=valorInicialPagina+((columnasMatriz*columnasComposicion*(filaMatriz)+columnaMatriz+columnasMatriz*(columnaComposicion)+(filaComposicion)*columnasComposicion*columnasMatriz*filasMatriz))*incremento[i];
                                                /*int numCifras=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                                                Vtemp=QString("%1").arg(valor_actual).rightJustified(numCifras,'0',true);
                                                qDebug() << "valor_actual barcode: " << valor_actual;*/
                                                counterStyle numEstilo;
                                                Vtemp=numEstilo.darformato(valor_actual,memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format);

                                            }
                                            valorActualFueraDeLimites(i);
                                        }
                                        //Si es un campo de origen BASE DE DATOS
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                            //A revisar
                                            /*int LocalOffset=index*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas*numCamposVariablesDB;
                                            int LocalIndex=i+(filaComposicion*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas+columnaComposicion)*numCamposVariablesDB;
                                            Vtemp=miDataBaseCampos.m_Tabla[LocalOffset+LocalIndex];*/

                                        }
                                        //Si es un campo de tipo Fecha/Hora
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                            QDateTime fechahora;
                                            QString sFechaHora;
                                            sFechaHora=fechahora.currentDateTime().toString(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato));
                                            Vtemp=QString("%1").arg(sFechaHora);
                                        }
                                        //Si es un campo de tipo Aleatorio
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                            int limiteInferior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior;
                                            int limiteSuperior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior;
                                            int numeroCifras=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras;
                                            In2Random In2Rand;
                                            int valorAleatorioActual=In2Rand.generarNumeroAleatorio(limiteInferior,limiteSuperior);
                                            Vtemp=QString("%1").arg(valorAleatorioActual).rightJustified(numeroCifras,'0',true);
                                        }
                                        else{
                                            Vtemp=QString("Origen?");
                                        }
                                        if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                            p.translate(Xtemp,Ytemp);
                                            //p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);

                                            struct m_BarcodeParameters  barcodeParameters=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                            m_barcode->setParams(barcodeParameters);
                                            m_barcode->SetPixmap(Vtemp);

                                            int Orientacion=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            QPixmap foo=m_barcode->qpxImagen.transformed(QTransform().rotate(Orientacion));
                                            p.drawPixmap(0,0,foo);

                                            //p.drawPixmap(0,0,m_barcode->qpxImagen);
                                            //delete barcode;
                                        }
                                        else {
                                            int Orientacion=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            p.translate(Xtemp,Ytemp/*+tamano_fuente_campo*/);
                                            QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                            QPixmap dest;
                                            if (Orientacion){
                                                dest=foo.transformed(QTransform().rotate(Orientacion));
                                                p.drawPixmap(0,0,dest);
                                                //this->drawHoverRect(p,dest.rect());
                                            }
                                            else{
                                                p.drawPixmap(0,0,foo);
                                                //this->drawHoverRect(p,foo.rect());
                                            }

                                        }
                                        //Guarda el valor actual
                                        memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo=Vtemp;
                                        p.restore();
                                    }
                                    Xtemp+=dix;
                                    celda++;
                                }
                                Ytemp+=diy;
                            }
                        }//columnaComposicion
                        //valor_inicial_fila+=incremento[i];
                        //memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila=valor_inicial_fila;
                    }//fila


                } //if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
            }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        } //if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
    } //for (i=0;i<NUM_CAMPOS;i++){

    p.end();



    //PixmapPlantilla.save(QString("test%1.bmp").arg(index),"BMP");
    if (format==QImage::Format_Mono)
        return PixmapPlantilla.convertToFormat(QImage::Format_Mono,Qt::MonoOnly);
    else {//De momento lo forzamos a indexado a 8 bit
        if (bHighSpeed){
            QImage tmpPixmap=PixmapPlantilla.scaled(PixmapPlantilla.width(),PixmapPlantilla.height()/2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            //p.ImageInfo.m_Imagen=tmpPixmap.toImage().convertToFormat(QImage::Format_Indexed8,table,Qt::AutoColor);
            return tmpPixmap.convertToFormat(QImage::Format_Indexed8,table,Qt::AvoidDither);
        }
        else
            return PixmapPlantilla.convertToFormat(QImage::Format_Indexed8,table,Qt::AvoidDither);
    }
    /*PixmapPlantilla=pic.transformed(QTransform().rotate(0,Qt::ZAxis));
    //PixmapPlantilla.save(QString("test%1.bmp").arg(index),"BMP");
    return PixmapPlantilla.toImage().convertToFormat(QImage::Format_Indexed8,table,Qt::AutoColor);*/

}






void ImgVarGenerator::controlContadorValorInicialPagina(unsigned char numCampo){

    if (memoryMap->m_mapa.m_camposvariables[numCampo].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
        if (indicePagina<=modulo_inicial[numCampo]){
            valorInicialPagina=memoryMap->m_mapa.m_camposvariables[numCampo].m_counterParameters.valorInicial
                    +incremento[numCampo]*(indicePagina)*impresionesPorPagina;
        }
        else{
            if (incremento[numCampo]>0)
                valorInicialPagina=limite_inferior[numCampo]+incremento[numCampo]*(indicePagina)*impresionesPorPagina;
            else
                valorInicialPagina=limite_superior[numCampo]+incremento[numCampo]*(indicePagina)*impresionesPorPagina;
        }
    }
    else{//if (memoryMap->m_mapa.m_camposvariables[numCampo].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){

        if (indicePagina<=modulo_inicial[numCampo]){
            valorInicialPagina=memoryMap->m_mapa.m_camposvariables[numCampo].m_counterParameters.valorInicial
                    +incremento[numCampo]*(indicePagina)*filasMatriz*filasComposicion;
        }
        else{
            if (incremento[numCampo]>0)
                valorInicialPagina=limite_inferior[numCampo]+incremento[numCampo]*(indicePagina)*filasMatriz*filasComposicion;
            else
                valorInicialPagina=limite_superior[numCampo]+incremento[numCampo]*(indicePagina)*filasMatriz*filasComposicion;
        }

    }

}

void ImgVarGenerator::valorActualFueraDeLimites(unsigned char numCampo){

    if (valor_actual>memoryMap->m_mapa.m_camposvariables[numCampo].m_counterParameters.limiteSuperior)
        valorActualOutOfLimits=true;
    else if (valor_actual<memoryMap->m_mapa.m_camposvariables[numCampo].m_counterParameters.limiteInferior)
        valorActualOutOfLimits=true;
    else
        valorActualOutOfLimits=false;

}


int ImgVarGenerator::calculoNuevoIndex(unsigned char numCampo){

    int dummy_modulo;

    if (indicePagina<=modulo_inicial[numCampo]){
        dummy_modulo=indicePagina%modulo_inicial[numCampo];
        if ((dummy_modulo==0)&&(indicePagina>0))indicePaginaModificado=modulo_inicial[numCampo];
        else indicePaginaModificado=dummy_modulo;
    }
    else{
        dummy_modulo=(indicePagina-modulo_inicial[numCampo])%modulo[numCampo];
        if (dummy_modulo==0) indicePaginaModificado=modulo[numCampo];
        else indicePaginaModificado=dummy_modulo;
    }

    return indicePaginaModificado;
}

//Crea cada una de las imagenes que forma la impresion
QImage ImgVarGenerator::CreaImagenVariableUnitaria(int index){

    unsigned char tamano_fuente_campo;
    unsigned int Xtemp,Ytemp;
    unsigned int Xotemp,Yotemp;
    unsigned int dix,diy;

    unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
    QString Vtemp;
    long dummy_index;


    QPixmap PixmapPlantilla(memoryMap->m_Imagen.width(),memoryMap->m_Imagen.height());
    QPixmap pic(PixmapPlantilla.size());
    QPainter p(&pic);   // create a QPainter for it
    pic.fill(Qt::white);
    QRect newRect=PixmapPlantilla.rect();
    newRect.setWidth((memoryMap->m_Imagen.width()/memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas));
    newRect.setHeight((memoryMap->m_Imagen.height()/memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas)-memoryMap->m_mapa.m_variables.SeparacionY+memoryMap->m_mapa.m_variables.PuntoInicialY);
    p.drawPixmap(0,0,memoryMap->m_Imagen.copy(newRect)); // and draw your original pixmap on it
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

    QStringList lista_excepciones;
    unsigned char indice_color=0;
    long valor_actual=0;
    long offset_columna=VARIABLE_OFFSET_COLUMNA;

    //int numCamposVariablesDB=miDataBaseCampos.CamposDatabase;
    //int numCamposVariables=NUM_CAMPOS_VARIABLES;
    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
            tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
            QFont mifont=configureFont(i);
            p.setFont(mifont);
            if (memoryMap->m_mapa.m_variables.NumColores==1){
                p.setPen(Qt::black);
            }
            else{
                indice_color= memoryMap->m_mapa.m_campos[i].color-1;
                p.setPen(memoryMap->m_mapa.m_colores[indice_color].valor);
            }
            dummy_index=(index-1)%limite_index[i];
            int numMatrices=memoryMap->getNumMatricesCampoVariable(i);
            for (int j=0;j<numMatrices/*NUM_MATRIZ_POR_CAMPO*/;j++){
                if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                    lista_excepciones.clear();
                    excepciones=QString(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    Xotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);
                    Yotemp=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                    dix=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(unsigned int) (FACTOR_PIXELS_MM*memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);
                            int filaComposicion=0;
                            int columnaComposicion=0;
                            Ytemp=Yo+Yotemp+filaComposicion*sy;
                            celda=1;
                            for (unsigned int filaMatriz=0;filaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                                Xtemp=Xo+Xotemp+columnaComposicion*sx;
                                for (unsigned int columnaMatriz=0;columnaMatriz<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                    celda_s=QString("%1").arg(celda);
                                    esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                    if (esunaexcepcion==false){
                                        p.save();
                                        p.translate(Xtemp,Ytemp+tamano_fuente_campo);
                                        p.rotate(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo);
                                        //Si es un campo de tipo CONTADOR
                                        if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                            //La variable es de tipo contador y hay que sacar su valor conforme a valorinicial, incemento...
                                            //=$O$1+(O$3-1)*$R$1+($M4-1)*$U$1+($L4-1)*$X$1
                                            //=ValorInicial+Offset*(Columna-1)+Incremento*(Fila-1)+Filas*(Index-1)
                                            valor_actual=valor_inicial[i]+offset_columna*columnaComposicion+incremento[i]*filaComposicion+memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*index;
                                            //Vtemp=QString("%1").arg(valor_actual).rightJustified(10,'0',true);
                                            counterStyle numEstilo;
                                            Vtemp=numEstilo.darformato(valor_actual,memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format);


                                            dummy_index++;

                                        }
                                        //Si es un campo de origen BASE DE DATOS
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                            //A revisar
                                            /*int LocalOffset=index*numCamposVariablesDB;
                                            int LocalIndex=i+(2*filaComposicion+columnaComposicion);
                                            Vtemp=miDataBaseCampos.m_Tabla[LocalOffset+LocalIndex];*/

                                        }
                                        //Si es un campo de tipo Fecha/Hora
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                            QDateTime fechahora;
                                            QString sFechaHora;
                                            sFechaHora=fechahora.currentDateTime().toString(QString("%1").arg(memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato));
                                            Vtemp=QString("%1").arg(sFechaHora);
                                        }
                                        //Si es un campo de tipo Aleatorio
                                        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                            int limiteInferior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior;
                                            int limiteSuperior=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior;
                                            int numeroCifras=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras;
                                            In2Random In2Rand;
                                            int valorAleatorioActual=In2Rand.generarNumeroAleatorio(limiteInferior,limiteSuperior);
                                            Vtemp=QString("%1").arg(valorAleatorioActual).rightJustified(numeroCifras,'0',true);
                                        }
                                        else{
                                            Vtemp=QString("Origen?");
                                        }
                                        if (!memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                            //Retrocedemos
                                            //Retrocedemos
                                            p.translate(0,(-1)*tamano_fuente_campo);
                                            struct m_BarcodeParameters  barcodeParameters=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                            m_barcode->setParams(barcodeParameters);
                                            m_barcode->SetPixmap(Vtemp);
                                            p.drawPixmap(0,0,m_barcode->qpxImagen);
                                        }
                                        else {
                                            int Orientacion=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            p.translate(Xtemp,Ytemp/*+tamano_fuente_campo*/);
                                            QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                            QPixmap dest;
                                            if (Orientacion){
                                                dest=foo.transformed(QTransform().rotate(Orientacion));
                                                p.drawPixmap(0,0,dest);
                                                //this->drawHoverRect(p,dest.rect());
                                            }
                                            else{
                                                p.drawPixmap(0,0,foo);
                                                //this->drawHoverRect(p,foo.rect());
                                            }
                                        }
                                        p.restore();
                                    }
                                    Xtemp+=dix;
                                    celda++;
                                }
                                Ytemp+=diy;
                            }

                } //if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
            }  //for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
        } //if (memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01){
    } //for (i=0;i<NUM_CAMPOS;i++){

    p.end();

    PixmapPlantilla=pic.transformed(QTransform().rotate(0,Qt::ZAxis));
    //PixmapPlantilla.save(QString("test%1.bmp").arg(index),"BMP");
    return PixmapPlantilla.toImage().convertToFormat(QImage::Format_Indexed8,table,Qt::AutoColor);

}



/** Devuelve un pixmap con un texto pintado*/
QPixmap ImgVarGenerator::returnPixmapFromText(QPainter &p,const QString & text){
    QFontMetrics metrics=p.fontMetrics();
    QSize size=metrics.size(Qt::TextSingleLine,text);
    QPixmap foo(size);
    foo.fill(Qt::transparent);
    QPainter painter(&foo);
    painter.setFont(p.font());
    painter.setPen(QColor(Qt::black));
    painter.drawText(0,size.height(),text);
    painter.end();

    return foo;
}
