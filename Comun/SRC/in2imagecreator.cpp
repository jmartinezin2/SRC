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
#include "in2imagecreator.h"
#include "In2BarcodeClass.h"
#include <QMessageBox>
#include <QFile>
#include <QBitmap>
#include <QFontMetrics>
#include <QMessageBox>
#include <QTransform>
#include "counterStyle.h"
#include <QColor>
#include <QFileInfo>
#include <QElapsedTimer>
#include "inkjetdebug.h"
#include "GsRenderEngine.h"
#include "in2random.h"
#include "in2font.h"
#include "cmykconfig.h"
#include "in2cmykgenerator.h"
#include "in2sabre.h"
#include "in2defines.h"
#include <QElapsedTimer>
#include "in2configuratordesigner.h"
#include "In2FloydSteinberg.h"
#include "In2WhiteCorrectionImage.h"
#if QT_VERSION>=0x050000
    #include <QtConcurrent>
#else
    #include <QtCore>
#endif




In2ImageCreator::In2ImageCreator(QObject *parent) :
    QObject(parent)
{
    m_memoryMap=MemoryMap::request(this);
    table.clear();
    for( int i = 0; i < 256; ++i ){
        table.append(qRgb(i,i,i));
    }
}
/** Genera la imagen*/
void In2ImageCreator::sltGenerateImage(){
    //QElapsedTimer fooTimer;
    //fooTimer.start();
    unsigned int j=0;
    unsigned int ImpresionesPosibles;

    if (m_memoryMap->m_mapa.m_variables.NumColores>NUM_COLORES){
        QMessageBox::warning(0,"ERROR",tr("Esta versión de SW solo permite %1 colores. Composición hecha para %2 colores").arg(NUM_COLORES).arg(m_memoryMap->m_mapa.m_variables.NumColores));
        return;
    }

    ImpresionesPosibles=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    emit sgnImgCrtInit();
    m_Impresiones=ImpresionesPosibles+1; //Cada render tiene un minimo de 5 pasos
    emit sgnImgCrtProgress(1,m_Impresiones);


    for(int indice_color=0;indice_color<m_memoryMap->m_mapa.m_variables.NumColores;indice_color++){

        pixMapArray[indice_color].clear();
    }
    QElapsedTimer time;
    time.start();

    //Calculo monohilo
    for(j=0;j<ImpresionesPosibles;j++){
        sltCreaImagenUnitariaColoresPlenos(j);
    }



    sltCreaImagenEnviarColoresPlenos();

    qDebug()<<QString("sltGenerateImage:%1").arg(time.elapsed());

    m_memoryMap->m_ComposicionGenerada=true;
    m_memoryMap->m_UltimaAccion=UA_CONFIGURACION_GENERADA;

    //Txemari revisa esto!!!!
    ///CopiarBBDD();
    if ((m_memoryMap->m_mapa.m_ColorMascara.x_pix!=DEFAULT_NOMASK_PIXEL)&& (m_memoryMap->m_mapa.m_ColorMascara.y_pix!=DEFAULT_NOMASK_PIXEL)){
        ///GeneraFicheroMascaraComposicion();
    }
    //emit sgnImgCrtEnd();

}
/** Genera la imagen*/
void In2ImageCreator::sltGenerateImageToPrint(){
    sltGenerateImage();
    /*unsigned int ImagenesPorEnvio=0;
    unsigned int j=0;
    unsigned int ImpresionesPosibles;

    if (m_memoryMap->m_mapa.m_variables.NumColores>NUM_COLORES){
        QMessageBox::warning(0,"ERROR",QString("Esta versión de SW solo permite %1 colores. Composición hecha para %2 colores").arg(NUM_COLORES).arg(m_memoryMap->m_mapa.m_variables.NumColores));
        return;
    }
    ImpresionesPosibles=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    ImagenesPorEnvio=ImpresionesPosibles;

    for(int indice_color=0;indice_color<m_memoryMap->m_mapa.m_variables.NumColores;indice_color++){
            pixMapArray[indice_color].clear();
    }

    for(j=0;j<ImpresionesPosibles;j++){
        if (j<ImagenesPorEnvio) sltCreaImagenFondo(j);
        else sltCreaImagenUnitariaEnBlanco(j);
    }

    sltCreaImagenEnviarColoresPlenos();

    m_memoryMap->m_ComposicionGenerada=true;
    m_memoryMap->m_UltimaAccion=UA_CONFIGURACION_GENERADA;

    //Txemari revisa esto!!!!
    ///CopiarBBDD();
    if ((m_memoryMap->m_mapa.m_ColorMascara.x_pix!=DEFAULT_NOMASK_PIXEL)&& (m_memoryMap->m_mapa.m_ColorMascara.y_pix!=DEFAULT_NOMASK_PIXEL)){
        ///GeneraFicheroMascaraComposicion();
    }*/

}

/** Aplica el dither seleccionado sobre una imagen*/
QImage In2ImageCreator::ditherImage(QImage & generatedImage,struct m_Colores & color){

    QImage rtnImage;

    QVector<QRgb> m_table256;
    m_table256.resize(256);
    for (int x=0;x<256;x++){
        m_table256[x]=qRgb(x,x,x);
    }

    In2FloydSteinberg fs;
    /*QVector<QRgb> Table8=fs.getOptimumTable(generatedImage,8);*/
    QString alg=color.alg;

    if (!alg.compare(TXT_DIFFUSE)){
        rtnImage=generatedImage.convertToFormat(QImage::Format_Indexed8,Qt::DiffuseDither);
        rtnImage.setColorTable(m_table256);


    }
    else if (!alg.compare(TXT_ORDERED)) {
        rtnImage=generatedImage.convertToFormat(QImage::Format_Indexed8,Qt::OrderedDither);
        rtnImage.setColorTable(m_table256);

    }
    else if (!alg.compare(TXT_THRESHOLD)){
        rtnImage=generatedImage.convertToFormat(QImage::Format_Indexed8,Qt::ThresholdDither);
        rtnImage.setColorTable(m_table256);
    }
    else if (!alg.compare(TXT_FLOYD)){
        //Solo tiene sentido si hay mas de 4 colores
        QStringList colorCount=color.cadena.split("@");
        if (colorCount.count()>4){
            QImage gray=generatedImage.convertToFormat(QImage::Format_Indexed8,table);
            generatedImage=gray.convertToFormat(QImage::Format_ARGB32);
            int cut=color.m_diherCut;
            bool corr=color.bCorrDinamica;
            QVector<QRgb> optTable;

            if (color.m_customPal){
                QStringList lst=color.m_ditherPal.split("@");
                for (int x=0;x<lst.count();x++){
                    optTable.append((QRgb)lst.at(x).toUInt());
                }
            }
            else{
                if (cut==CUT_LINEAL)
                    optTable=fs.getOptimumTable(generatedImage,4,corr);
                else if (cut==CUT_MEDIAN)
                    optTable=fs.getOptimumTableMediaCut(generatedImage,4,corr);
                else
                    optTable=fs.getOptimumTableIn2(generatedImage,4,corr);
            }


            int count=optTable.count();
            if (count<4){
                QRgb colorRGB=0xFFFFFFFF;
                qDebug()<<QString("Color table solo de %1 colores...rellenando").arg(optTable.count());
                for (int x=0;x<4-count;x++){
                    optTable.append(colorRGB);
                }
            }
            color.m_ditherPal=QString("%1@%2@%3@%4").arg(QString::number(optTable.at(0))).arg(QString::number(optTable.at(1))).arg(QString::number(optTable.at(2))).arg(QString::number(optTable.at(3)));
            for (int x=0;x<4;x++){
                qDebug()<<QString("C%1 %2").arg(x).arg(QString::number(optTable.at(x),16));
            }
            rtnImage=fs.convert(generatedImage,optTable); //Vamos a dejarlo en 4 nivels

        }
        else {
            emit SendDebugInfo(LOGGER_ERROR,QString("No hay suficientes colores seleccionados, no se aplica el dither"));
            rtnImage=generatedImage;
        }
        //En principio son todo grises...
    }
    else rtnImage=generatedImage;

    if (color.bMonoMode){
        QImage fooImage=rtnImage.convertToFormat(QImage::Format_Mono,Qt::DiffuseDither);
        rtnImage=fooImage.convertToFormat(QImage::Format_Indexed8,table,Qt::DiffuseDither);
    }
    if (color.bWhiteCorr){
        In2WhiteCorrectionImage WhiteCorr;
        WhiteCorr.correctWhite(generatedImage,&rtnImage);
    }
    if (color.bMonoCorr){
        if (!generatedImage.isNull()){
            //TODO correccion mono
        }
    }
    if (color.bBlackCorr){
        //TODO correccion black
    }

    emit sgnDitherCompleted();

    return rtnImage;
}

/** Crea cada una de las imagenes que forma la impresion*/
void In2ImageCreator::sltCreaImagenUnitariaEnBlanco(unsigned int indice){

}

/** Crea cada una de las imagenes que forma la impresión*/
void In2ImageCreator::sltCreaImagenUnitariaColoresPlenos(unsigned int indice){

    //unsigned char tamano_fuente_campo;
    float Xtemp,Ytemp;
    float dix,diy;
    float incX,incY;
    unsigned int celda;
    QString celda_s;
    QString excepciones;

    QStringList lista_excepciones;
    QString Vtemp;

    for(unsigned char indice_color=0;indice_color<m_memoryMap->m_mapa.m_variables.NumColores;indice_color++){


        QString dummy=QString(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
        dummy.replace(QString(".bmp"),QString("_C%1.bmp").arg(indice_color+1));

        QImage generatedImage(dummy);
        struct m_Colores color=m_memoryMap->m_mapa.m_colores[indice_color];
        QImage rtnImage=ditherImage(generatedImage,color);
        m_memoryMap->m_mapa.m_colores[indice_color]=color;
        QImage pic=rtnImage.convertToFormat(QImage::Format_ARGB32);

        QPainter p(&pic);
        p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

        //pic.fill(QColor(Qt::white).rgb());
        //pic.fill()

        if (m_memoryMap->m_mapa.m_variables.NumColores==1){
            p.setPen(Qt::black);
        }
        else{
            ///p.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
            p.setPen(Qt::black);
        }
        /*if (!pixmapUnitario->isNull())
            p.drawPixmap(0,0,*pixmapUnitario);


        delete (pixmapUnitario);*/

        for (int i=0;i<NUM_CAMPOS;i++){

            if ((m_memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01) &&
                    (m_memoryMap->m_mapa.m_campos[i].color==(indice_color+1))) {
                for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                    if (m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
                        lista_excepciones.clear();
                        excepciones=QString(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
                        lista_excepciones=excepciones.split(",");
                        dix=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo);
                        diy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo);
                        incX=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas);
                        incY=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas);
                        Vtemp=QString(/*m_memoryMap->m_mapa.m_campos[i].ValorActualCampo*/m_memoryMap->m_mapa.m_campos[i].ValorActualCampo);
                        QFont mifont=_setFont(i,true);
                        p.setFont(mifont);
                        Ytemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo);
                        celda=1;
                        for (unsigned int filaMatriz=0;filaMatriz<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                            Xtemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo);
                            for (unsigned int columnaMatriz=0;columnaMatriz<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                float YMod=Ytemp+incY*columnaMatriz;
                                celda_s=QString("%1").arg(celda);
                                bool esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                if (esunaexcepcion==false){
                                    p.save();
                                    if (!m_memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                        //Nos movemos menos...
                                        struct m_BarcodeParameters bc=m_memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
                                        int Orientacion=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
                                        p.translate(Xtemp,YMod);
                                        drawBarcode(p,bc,Orientacion,Vtemp);
                                    }
                                    else{
                                        int Orientacion=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
                                        p.translate(Xtemp,YMod);
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
                                Xtemp+=dix+columnaMatriz*incX;
                                celda++;
                            }
                            Ytemp+=diy;
                        }
                    }
                }
            }

        } //for (i=0;i<NUM_CAMPOS;i++)

        for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
            if (m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
                //tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
                QFont mifont=_setFont(i,false);
                p.setFont(mifont);

                if (m_memoryMap->m_mapa.m_variables.NumColores==1){
                    p.setPen(Qt::black);
                }
                else{
                    indice_color= m_memoryMap->m_mapa.m_campos[i].color-1;
                    //p.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
                    p.setPen(Qt::black);
                }

                for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                    if (m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                        lista_excepciones.clear();
                        excepciones=QString(m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                        lista_excepciones=excepciones.split(",");
                        dix=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                        diy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);
                        incX=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas);
                        incY=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas);

                        Ytemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                        celda=1;
                        for (unsigned int filaMatriz=0;filaMatriz<m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                            Xtemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);

                            for (unsigned int columnaMatriz=0;columnaMatriz<m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                float YMod=Ytemp+incY*columnaMatriz;
                                celda_s=QString("%1").arg(celda);
                                bool esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                if (esunaexcepcion==false){
                                    p.save();
                                    //Si es un campo de tipo CONTADOR
                                    if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                        Vtemp=_getCounterValue(i,filaMatriz,columnaMatriz,0);
                                    }
                                    //Si es un campo de origen BASE DE DATOS
                                    else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                        Vtemp=_getBBDDValue(i);
                                    }
                                    //Si es un campo de tipo Fecha/Hora
                                    else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                        Vtemp=_getDateTimeValue(i);
                                    }
                                    //Si es un campo de tipo Aleatorio
                                    else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                        Vtemp=_getRandomValue(i);
                                    }
                                    else{
                                        Vtemp=QString("Origen?");
                                    }
                                    if (!m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                        int Orientacion=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                        p.translate(Xtemp,YMod);
                                        struct m_BarcodeParameters bc=m_memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                        drawBarcode(p,bc,Orientacion,Vtemp);
                                    }
                                     else {
                                        int Orientacion=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                        p.translate(Xtemp,YMod);
                                        QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                        QPixmap dest;
                                        if (Orientacion){
                                            dest=foo.transformed(QTransform().rotate(Orientacion));
                                            p.drawPixmap(0,0,dest);
                                         }
                                        else{
                                            p.drawPixmap(0,0,foo);
                                        }
                                     }
                                     p.restore();
                                   }
                                   Xtemp+=dix+columnaMatriz*incX;
                                   celda++;
                                }
                                Ytemp+=diy;
                            }
                        }
                    }
                }
            }

        p.end();
        pixMapArray[indice_color].append(pic);
    }
    emit sgnImgCrtProgress(1,m_Impresiones);
}
/** Devuelve valor aleatorio*/
QString In2ImageCreator::_getRandomValue(int campo){
    int limiteInferior=m_memoryMap->m_mapa.m_camposvariables[campo].m_randomNumberParameters.limiteInferior;
    int limiteSuperior=m_memoryMap->m_mapa.m_camposvariables[campo].m_randomNumberParameters.limiteSuperior;
    int numeroCifras=m_memoryMap->m_mapa.m_camposvariables[campo].m_randomNumberParameters.numeroCifras;
    In2Random in2Ran;
    int valorAleatorioActual=in2Ran.generarNumeroAleatorio(limiteInferior,limiteSuperior);
    QString Vtemp=QString("%1").arg(valorAleatorioActual).rightJustified(numeroCifras,'0',true);
    return Vtemp;
}


/** Devuelve valor de fecha/hora*/
QString In2ImageCreator::_getDateTimeValue(int campo){
    QDateTime fechahora;
    QString sFechaHora=fechahora.currentDateTime().toString(QString("%1").arg(m_memoryMap->m_mapa.m_camposvariables[campo].m_dateTimeParameters.formato));
    return sFechaHora;
}

/** Devuelvge el valor de la BBDD que le corresponde*/
QString In2ImageCreator::_getBBDDValue(int campo){
    //A revisar
    /*int index=0;
    int numCamposVariablesDB=0;
    int LocalOffset=index*m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas*numCamposVariablesDB;*/
    //int LocalIndex=campo+(1/*fila*/*m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas+1/*columna*/)*numCamposVariablesDB;
    //QString Vtemp=miDataBaseCampos.m_Tabla[LocalOffset+LocalIndex];
    //return Vtemp;
    return QString();
}

/** Devuelve el contador que correspone*/
QString In2ImageCreator::_getCounterValue(int campo,int fila, int columna,int impresion){
    qint64 valorInicialPagina=0;
    qint64 valor_actual=0;
    qint64 offset_columna=m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.offset;
    qint64 incremento=m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.incremento;

    if (m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
        qint64 OffsetPagina=impresion*m_memoryMap->m_mapa.m_camposvariables[campo].m_matrices[0].FilasMatrizCampo;
        valorInicialPagina=m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.valorInicial+OffsetPagina;
        valor_actual= valorInicialPagina+(fila)*incremento+offset_columna*(columna);
     }
     else if (m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
        qint64 OffsetPagina=impresion*m_memoryMap->m_mapa.m_camposvariables[campo].m_matrices[0].ColumnasMatrizCampo;
        valorInicialPagina=m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.valorInicial+OffsetPagina;
        valor_actual= valorInicialPagina+((columna))*incremento+offset_columna*(fila);
     }
    if (valor_actual<m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.limiteInferior) valor_actual=m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.limiteInferior;
    else if (valor_actual>m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.limiteSuperior) valor_actual=m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.limiteSuperior;
    counterStyle numEstilo;
    QString Vtemp=numEstilo.darformato(valor_actual,m_memoryMap->m_mapa.m_camposvariables[campo].m_counterParameters.m_format);
    return Vtemp;
}

/** Carga una fuente*/
QFont In2ImageCreator::_setFont(int i, bool campoFijo){
    QFont miFont;
    if (!campoFijo){
        unsigned char tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
        In2Font in2Font;
        int peso_campo=in2Font.getQtFontWeight(m_memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo);
        miFont.setPixelSize(tamano_fuente_campo);
        miFont.setStyleStrategy(QFont::PreferAntialias);
        miFont.setFamily(m_memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo);
        miFont.setWeight(peso_campo);
        miFont.setLetterSpacing(QFont::AbsoluteSpacing,m_memoryMap->m_mapa.m_camposvariables[i].letterSpace);
    }
    else{
        unsigned char tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*m_memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo);
        In2Font in2Font;
        int peso_campo=in2Font.getQtFontWeight(m_memoryMap->m_mapa.m_campos[i].EstiloLetraCampo);
        miFont.setPixelSize(tamano_fuente_campo);
        miFont.setStyleStrategy(QFont::PreferAntialias);
        miFont.setFamily(m_memoryMap->m_mapa.m_campos[i].TipoLetraCampo);
        miFont.setWeight(peso_campo);
        miFont.setLetterSpacing(QFont::AbsoluteSpacing,m_memoryMap->m_mapa.m_campos[i].letterSpace);

    }
    return miFont;
}

/** Crea cada una de las imagenes que forma la impresión*/
void In2ImageCreator::sltCreaImagenFondo(unsigned int indice){

    unsigned char indice_color=0;
    float Xtemp,Ytemp;
    float Xotemp,Yotemp;
    float dix,diy;
    float incX,incY;
    unsigned int celda;
    QString celda_s;
    QString excepciones;
    QStringList lista_excepciones;
    QString Vtemp;

    for(indice_color=0;indice_color<m_memoryMap->m_mapa.m_variables.NumColores;indice_color++){

        QString dummy=QString(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
        dummy.replace(QString(".bmp"),QString("_C%1.bmp").arg(indice_color+1));
        QPixmap *pic=new QPixmap(dummy,"BMP",Qt::MonoOnly);
        QPainter p(pic);
        p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

        p.setPen(Qt::black);

        for (int i=0;i<NUM_CAMPOS;i++){
            if ((m_memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01) &&
                    (m_memoryMap->m_mapa.m_campos[i].color==(indice_color+1))) {
                for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                    if (m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
                        lista_excepciones.clear();
                        excepciones=QString(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
                        lista_excepciones=excepciones.split(",");
                        Xotemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo);
                        Yotemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo);
                        dix=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo);
                        diy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo);
                        incX=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas);
                        incY=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas);

                        Vtemp=QString(/*m_memoryMap->m_mapa.m_campos[i].ValorActualCampo*/m_memoryMap->m_mapa.m_campos[i].ValorActualCampo);
                        QFont mifont=_setFont(i,true);
                        p.setFont(mifont);
                        Ytemp=Yotemp;
                        celda=1;
                        for (unsigned int filaMatriz=0;filaMatriz<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                            Xtemp=Xotemp;
                            for (unsigned int columnaMatriz=0;columnaMatriz<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                float YMod=Ytemp+incY*columnaMatriz;
                                celda_s=QString("%1").arg(celda);
                                bool esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                if (esunaexcepcion==false){
                                    p.save();
                                    if (!m_memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                        //Nos movemos menos...
                                        p.translate(Xtemp,YMod);
                                        struct m_BarcodeParameters bc=m_memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
                                        In2BarcodeClass barcodeInst;
                                        barcodeInst.setParams(bc);
                                        barcodeInst.SetPixmap(Vtemp);
                                        int Orientacion=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
                                        QPixmap foo=barcodeInst.qpxImagen.transformed(QTransform().rotate(Orientacion));
                                        p.drawPixmap(0,0,foo);
                                    }
                                    else{
                                        int Orientacion=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
                                        p.translate(Xtemp,YMod);
                                        QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                        QPixmap dest;
                                        if (Orientacion){
                                            dest=foo.transformed(QTransform().rotate(Orientacion));
                                            p.drawPixmap(0,0,dest);
//                                            this->drawHoverRect(p,dest.rect());
                                        }
                                        else{
                                            p.drawPixmap(0,0,foo);
//                                            this->drawHoverRect(p,foo.rect());
                                        }
                                    }
                                     p.restore();
                                }
                                Xtemp+=dix+columnaMatriz*incX;
                                celda++;
                            }
                            Ytemp+=diy;
                        }
                    }
                }
            }
        } //for (i=0;i<NUM_CAMPOS;i++)

        pixMapArray[indice_color].append(pic->toImage());
        p.end();
        delete (pic);

    }

}




/** Genera el fichero de máscara*/
void In2ImageCreator::GeneraFicheroMascaraComposicion(){

        int ipef=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
        int ipec=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
        int NumImpresiones=ipef*ipec;

        int indice=0;
        float ww=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.WebWidth);
        float wh=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.WebHeight);
        /*unsigned int x=(unsigned int)(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.PuntoInicialX);
        unsigned int y=(unsigned int)(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.PuntoInicialY);*/
        float x=0;
        float y=0;
        float dix=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.SeparacionX);
        float diy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.SeparacionY);

        QPixmap pic(ww,wh);
        QPainter p(&pic);   // create a QPainter for it
        pic.fill(Qt::white);


        QPixmap rotatedpixmap;
        rotatedpixmap.load(m_memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::AutoColor);
        QPixmap aux=rotatedpixmap.transformed(QTransform().rotate(m_memoryMap->m_mapa.m_variables.AnguloPlantilla,Qt::ZAxis));

        for(int i=0;i<ipef;i++){
            for(int j=0;j<ipec;j++){
                if (indice<NumImpresiones) p.drawPixmap(x,y, aux);
                x=x+dix;//cp+sx;
                indice++;
            }
            x=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.PuntoInicialX);
            y=y+diy;//fp+sy;
        }

        QString dummyString=QString(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
        dummyString.replace(".bmp",QString("%1CompMask.bmp").arg(m_memoryMap->m_mapa.m_variables.OrdenFabricacion));
        ///pic.save(dummyString,"BMP");
}

/** Copia la BBDD a la carpeta de Composicion*/
void In2ImageCreator::CopiarBBDD(){
    _CopiarBBDD();
}
/** Antiguao _copiarBBDD de globals*/
QString In2ImageCreator::_CopiarBBDD(){
    QString rtnName;
    /*if (!memoryMap->m_mapa.m_database.NombreRutaDb.isEmpty()){
        QFile inputFile(memoryMap->m_mapa.m_database.NombreRutaDb);
            if (inputFile.exists()){
                QFileInfo fileInfo(inputFile.fileName());
                QString name(fileInfo.fileName());
                rtnName=QString(memoryMap->m_mapa.m_variables.NombreRutaIni);
                rtnName.remove(".ini");
                rtnName.append(QString("%1").arg(memoryMap->getOrdenAndProducto()));
                rtnName.append(name);
                QFile tmp(rtnName);
                if (tmp.exists()) tmp.remove();
                inputFile.copy(rtnName); //Creamos tabla de repeticiones
                miDataBaseCampos.close();
                if (miDataBaseCampos.open(rtnName)){
    #if QT_VERSION >= 0x050000
                    if (!miDataBaseCampos.execNoSave(QString("alter table DatosImprimir add column ImpresionOK varchar (255)").toLatin1().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo crear ImpresionOK en tabla DatosImprimir",QMessageBox::Ok);
                    }
                    if (!miDataBaseCampos.execNoSave(QString("alter table DatosImprimir add column In2Index counter").toLatin1().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo indexar tabla DatosImprimir",QMessageBox::Ok);
                    }
                    if (!miDataBaseCampos.execNoSave(QString("CREATE TABLE \"Repeticiones\" (Item varchar(255),Motivo varchar(255), Comentario varchar(255),Repeticiones varchar (255), Destruccion varchar (255), Fecha varchar (255))").toLatin1().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo crear tabla de repeticiones",QMessageBox::Ok);
                    }
                    if (!miDataBaseCampos.execNoSave(QString("CREATE TABLE \"FixedData\" (Campo varchar(255),Valor varchar(255))").toLatin1().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo crear tabla de repeticiones",QMessageBox::Ok);
                    }

    #else
                    if (!miDataBaseCampos.execNoSave(QString("alter table DatosImprimir add column ImpresionOK varchar (255)").toAscii().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo crear ImpresionOK en tabla DatosImprimir",QMessageBox::Ok);
                    }
                    if (!miDataBaseCampos.execNoSave(QString("alter table DatosImprimir add column In2Index counter").toAscii().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo indexar tabla DatosImprimir",QMessageBox::Ok);
                    }
                    if (!miDataBaseCampos.execNoSave(QString("CREATE TABLE \"Repeticiones\" (Item varchar(255),Motivo varchar(255), Comentario varchar(255),Repeticiones varchar (255), Destruccion varchar (255), Fecha varchar (255))").toAscii().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo crear tabla de repeticiones",QMessageBox::Ok);
                    }
                    if (!miDataBaseCampos.execNoSave(QString("CREATE TABLE \"FixedData\" (Campo varchar(255),Valor varchar(255))").toAscii().data())){
                        QMessageBox::warning(0,"ERROR!","No se pudo crear tabla de repeticiones",QMessageBox::Ok);
                    }
    #endif
                    miDataBaseCampos.close();
                }
                else QMessageBox::warning(0,"ERROR!","No se pudo generar BBDD",QMessageBox::Ok);
            }
        }*/
    return rtnName;
}





/** Crea la imagen a enviar*/
void In2ImageCreator::sltCreaImagenEnviarColoresPlenos(){

    unsigned char indice_color;
    unsigned int i,j;
    unsigned int x;
    unsigned int y;
    int indice;

    float wh,ww;
    unsigned int ipef,ipec;
    float x0,y0;
    float dix,diy;

    double ww_mm,wh_mm;
    double x0_mm,y0_mm;
    double dix_mm,diy_mm;

     QElapsedTimer myTime;
     myTime.start();


    //Siempre fijo
    unsigned int canvas_width=m_memoryMap->m_mapa.m_variables.DefaultNumHeads*DEFAULT_HEAD_WIDTH;
    emit SendDebugInfo(LOGGER_WARNING, QString("Generando %1 pixeles").arg(canvas_width));

    ww_mm=m_memoryMap->m_mapa.m_variables.WebWidth;
    wh_mm=m_memoryMap->m_mapa.m_variables.WebHeight;

    dix_mm=m_memoryMap->m_mapa.m_variables.SeparacionX;
    diy_mm=m_memoryMap->m_mapa.m_variables.SeparacionY;


    ipef=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
    ipec=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    x0_mm=m_memoryMap->m_mapa.m_variables.PuntoInicialX;
    y0_mm=m_memoryMap->m_mapa.m_variables.PuntoInicialY;
    /*x0_mm=0;
    y0_mm=0;*/


    wh=(FACTOR_PIXELS_MM*wh_mm);
    ww=(FACTOR_PIXELS_MM*ww_mm);


    dix=(FACTOR_PIXELS_MM*dix_mm);
    diy=(FACTOR_PIXELS_MM*diy_mm);


    x0=(FACTOR_PIXELS_MM*x0_mm);
    y0=(FACTOR_PIXELS_MM*y0_mm);


    //Dibujar maculas
    double mi_x_mm=m_memoryMap->m_mapa.m_variables.MaculaIzqdaX;
    double mi_y_mm=m_memoryMap->m_mapa.m_variables.MaculaIzqdaY;
    double mi_ancho_mm=m_memoryMap->m_mapa.m_variables.MaculaIzqdaAncho;
    double mi_alto_mm=m_memoryMap->m_mapa.m_variables.MaculaIzqdaAlto;
    double md_x_mm=m_memoryMap->m_mapa.m_variables.MaculaDchaX;
    double md_y_mm=m_memoryMap->m_mapa.m_variables.MaculaDchaY;
    double md_ancho_mm=m_memoryMap->m_mapa.m_variables.MaculaDchaAncho;
    double md_alto_mm=m_memoryMap->m_mapa.m_variables.MaculaDchaAlto;
    double mi_frecuencia_mm=m_memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia;
    double md_frecuencia_mm=m_memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia;

    md_x_mm = ww_mm - (md_x_mm+md_ancho_mm);

    float mi_xo;
    float mi_yo;
    float mi_x;
    float mi_y;
    float mi_ancho;
    float mi_alto;
    float md_xo;
    float md_yo;
    float md_x;
    float md_y;
    float md_ancho;
    float md_alto;
    float mi_frecuencia;
    float md_frecuencia;
    float num_mi;

    num_mi=m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones;

    mi_xo=(FACTOR_PIXELS_MM*mi_x_mm);
    mi_yo=(FACTOR_PIXELS_MM*mi_y_mm);
    mi_ancho=(FACTOR_PIXELS_MM*mi_ancho_mm);
    mi_alto=(FACTOR_PIXELS_MM*mi_alto_mm);
    mi_frecuencia=(FACTOR_PIXELS_MM*mi_frecuencia_mm);

    md_xo=(FACTOR_PIXELS_MM*md_x_mm);
    md_yo=(FACTOR_PIXELS_MM*md_y_mm);
    md_ancho=(FACTOR_PIXELS_MM*md_ancho_mm);
    md_alto=(FACTOR_PIXELS_MM*md_alto_mm);
    md_frecuencia=(FACTOR_PIXELS_MM*md_frecuencia_mm);

    //QPixmap dummy;
    QString nombreruta_bmp;


    int maxDim=wh;
    if (ww>wh) maxDim=ww;
    for(indice_color=0;indice_color<m_memoryMap->m_mapa.m_variables.NumColores;indice_color++){
        QImage pic=QImage(canvas_width,maxDim,QImage::Format_ARGB32);
        QPainter painter;
        painter.begin(&pic);
        // create a QPainter for it
        ///painter.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
        painter.setPen(Qt::black);
        //pic.fill(Qt::white);
#if QT_VERSION>=0x050000
        pic.fill(Qt::white);
#else
        pic.fill(QColor(Qt::white).rgb());
#endif


        //Para el color indice_color
        x=x0;
        y=y0;
        indice=0;

        for(i=0;i<ipef;i++){
            for(j=0;j<ipec;j++){                
                if (m_memoryMap->m_mapa.m_variables.AnguloPlantilla){
                    QImage foo=pixMapArray[indice_color].at(indice).transformed(QTransform().rotate(m_memoryMap->m_mapa.m_variables.AnguloPlantilla));
                    painter.drawImage(x,y, foo);
                }
                else{
                    painter.drawImage(x,y, pixMapArray[indice_color].at(indice));
                }
                x=x+dix;//cp+sx;
                indice++;
            }
            x=x0;
            y=y+diy;//fp+sy;
        }

        /*painter.end();
        struct m_Colores color=m_memoryMap->m_mapa.m_colores[indice_color];
        QImage fooImage=ditherImage(pic,color);
        pic=fooImage;
        painter.begin(&pic);*/


        //Maculas...siempre de color negro
        //QColor Color=QColor(m_memoryMap->m_mapa.m_colores[indice_color].valor);
        QColor Color=QColor(Qt::black);
        QBrush b=QBrush(Color,Qt::SolidPattern);
        ///painter.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
        painter.setPen(Qt::black);
        painter.setBrush(b);

        if (m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff==1){
            mi_x=mi_xo;
            mi_y=mi_yo;
            for (unsigned int n=0;n<num_mi;n++){
                if (m_memoryMap->m_mapa.m_variables.MaculaIzqdaColor==(indice_color+1)) painter.fillRect(mi_x,mi_y,mi_ancho,mi_alto,b);
                mi_y+=mi_frecuencia;
            }
        }

        num_mi=m_memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones;
        if (m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff==1){
            md_x=md_xo;
            md_y=md_yo;
            for (unsigned int n=0;n<num_mi;n++){
                if (m_memoryMap->m_mapa.m_variables.MaculaDchaColor==(indice_color+1)) painter.fillRect(md_x,md_y,md_ancho,md_alto,b);
                md_y+=md_frecuencia;
            }
        }

        //PharmaCodes
        struct strcMarca marca=m_memoryMap->m_mapa.m_Marca;
        if (marca.MarcaSupOnOff){
            painter.save();
            painter.translate(marca.MarcaSupX*FACTOR_PIXELS_MM,marca.MarcaSupY*FACTOR_PIXELS_MM);
            if (marca.MarcaSupTipo==0){
                struct m_BarcodeParameters bc=marca.barcodeSup;
                int angle=marca.anguloSup;
                drawBarcode(painter,bc,angle,marca.MarcaSupValor);
                painter.restore();
            }
            else if (marca.MarcaSupTipo==1){
                QPixmap foo=this->returnPixmapFromText(painter,marca.MarcaSupValor);
                if (marca.anguloSup){
                    QPixmap dest=foo.transformed(QTransform().rotate(marca.anguloSup));
                    painter.drawPixmap(0,0,dest);
                 }
                else{
                    painter.drawPixmap(0,0,foo);
                }
            }
        }
        if (marca.MarcaInfOnOff){
            painter.save();
            painter.translate(marca.MarcaInfX*FACTOR_PIXELS_MM,marca.MarcaInfY*FACTOR_PIXELS_MM);
            if (marca.MarcaInfTipo==0){
                struct m_BarcodeParameters bc=marca.barcodeInf;
                int angle=marca.anguloInf;
                drawBarcode(painter,bc,angle,marca.MarcaInfValor);
                painter.restore();
            }
            else{
                QPixmap foo=this->returnPixmapFromText(painter,marca.MarcaInfValor);
                if (marca.anguloInf){
                    QPixmap dest=foo.transformed(QTransform().rotate(marca.anguloInf));
                    painter.drawPixmap(0,0,dest);
                 }
                else{
                    painter.drawPixmap(0,0,foo);
                }
            }
        }
        //p2.end();
        painter.end();
        QImage *rotatedimage=new QImage(pic);
        if (!rotatedimage->isNull()){

            QString nombreruta_ini=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
            nombreruta_bmp=nombreruta_ini;
            nombreruta_bmp.remove(".ini");
            nombreruta_bmp.append(QString("_OF%1").arg(m_memoryMap->m_mapa.m_variables.OrdenFabricacion));
            nombreruta_bmp.append(QString("_C%1_comp.bmp").arg(indice_color+1));
            QImage *generatedImage=new QImage(rotatedimage->convertToFormat(QImage::Format_Indexed8,table,Qt::ColorOnly).copy(0,0,canvas_width,wh));
            delete (rotatedimage);
            generatedImage->setDotsPerMeterX(14174);
            generatedImage->setDotsPerMeterY(14174);
            QFile dummyFile(nombreruta_bmp);
            dummyFile.remove();

            generatedImage->save(nombreruta_bmp,"BMP");

        }
        else{
        }
    }



    nombreruta_bmp.replace(QString("_C%1_comp.bmp").arg(indice_color),QString("_C1_comp.bmp"));
    m_memoryMap->m_FilePath=nombreruta_bmp;
    //m_memoryMap->m_ImagenFijaEnviar=genImageToSend[0];
    m_memoryMap->m_Imagen.load(nombreruta_bmp);
    ///Hay que sacarlo fuera!
    //if (ui->m_dilate->isChecked()) PreprocessImagesIn2();
    if (m_memoryMap->m_mapa.m_variables.iSabreAlgorithm) sltSabreAlgorithm();
    if (m_memoryMap->m_mapa.m_variables.bDilateAlgorithm) sltDilateImage();
}
/** Aplica algoritmo dilate*/
void In2ImageCreator::sltSabreAlgorithm(){
    QString name=m_memoryMap->m_FilePath;
    name.remove(QString("_C1_comp.bmp"));
    name.append(QString("_C1_comp.bmp"));
    QFile file(name);
    if (file.exists()){
        In2Sabre sbr;
        //QImage rtn=lienzo;
        QImage rtn=sbr.getSabreImage(QImage(name),m_memoryMap->m_mapa.m_variables.iSabreAlgorithm);
        if (!rtn.isNull()){
            rtn.save(name);
            m_memoryMap->m_Imagen=QPixmap::fromImage(rtn);
        }
    }

}


/** Aplica algoritmo dilate*/
void In2ImageCreator::sltDilateImage(){
    QString StringBlack=m_memoryMap->m_FilePath;
    StringBlack.remove(QString("_C1_comp.bmp"));
    StringBlack.append(QString("_C1_comp.bmp"));
    QString StringRed=m_memoryMap->m_FilePath;
    StringRed.remove(QString("_C1_comp.bmp"));
    StringRed.append(QString("_C2_comp.bmp"));
    QString StringBase=m_memoryMap->m_FilePath;
    StringBase.remove("_C1_comp.bmp");
    StringBase.append("_C1.bmp");
    QFile fileBlack(StringBlack);
    QFile fileRed(StringRed);
    QFile fileBase(StringBase);

    if (fileBlack.exists()&&fileRed.exists()/*&&fileBase.exists()*/){
        QImage ImageBlack(StringBlack);
        QImage ImageRed(StringRed);
        //QImage ImageBase(StringBase);
        QRgb baseColor=QColor(Qt::white).rgb();
        //Buscamos primera ocurrencia en el rojo
        QRgb destColor=QColor(Qt::white).rgb();
        QRgb grdColor=QColor(Qt::white).rgb();
        for (int x=0;x<ImageRed.width();x++){
            if (grdColor!=destColor) break;
            for (int y=0;y<ImageRed.height();y++){
                if (ImageRed.pixel(x,y)!=destColor){
                    destColor=ImageRed.pixel(x,y);
                    break;
                }
            }
        }

        QVector<QRgb> tabla=ImageRed.colorTable();

        int DestIndex=0;
        for(DestIndex=0;DestIndex<tabla.size();DestIndex++){
            if (destColor==tabla.at(DestIndex)) break;
        }

        for (int x=1;x<ImageBlack.width()-1;x++) {
            for (int y=1;y<ImageBlack.height()-1;y++){
                if (ImageBlack.pixel(x,y)!=baseColor){
                    if ((ImageRed.pixel(x-1,y)!=baseColor)||(ImageRed.pixel(x+1,y)!=baseColor)||(ImageRed.pixel(x,y+1)!=baseColor)||(ImageRed.pixel(x,y-1)!=baseColor)){
                        ImageRed.setPixel(x,y,DestIndex);
                    }
                }
            }
        }
        ImageRed.save(StringRed);
    }
}

/** Carga la plantilla*/
void In2ImageCreator::CargaPlantilla(){
    Convert();
}
/** El antiguo convert de globals*/
void In2ImageCreator::Convert(){
    InkjetDebug *infoDebug=InkjetDebug::request(0);
    int error=0;
    unsigned int x0=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.XinicialPlantilla);
    unsigned int y0=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.YinicialPlantilla);
    unsigned int w=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.AnchoPlantilla);
    unsigned int h=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.AltoPlantilla);
    if (!QFile::exists(m_memoryMap->m_mapa.m_variables.NombreRutaPdf)&&(!m_memoryMap->m_mapa.m_variables.NombreRutaPdf.isEmpty())){
        QString foo=QString("%1\\%2").arg(m_memoryMap->m_mapa.m_variables.savedRutaIni).arg(m_memoryMap->m_mapa.m_variables.NombrePdf);
        if (QFile::exists(foo)){
            int rtn=QMessageBox::warning(0,"INFO",tr("No se encuentra %1 , pero se ha encontrado %2. Quiere cargar este archivo?").arg(m_memoryMap->m_mapa.m_variables.NombreRutaPdf).arg(foo),QMessageBox::Ok,QMessageBox::Cancel);
            if (rtn==QMessageBox::Ok){
                QFileInfo info(foo);
                m_memoryMap->m_mapa.m_variables.NombreRutaPdf=foo;
                m_memoryMap->m_mapa.m_variables.NombrePdf=info.fileName();
            }
            else return;
        }
        else{
            QMessageBox::information(0,"INFO",tr("No se encuentra %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaPdf),QMessageBox::Ok);
            return;
        }
    }
    QString dummy_bmp=QString("%1\\dummy.bmp").arg(m_memoryMap->m_mapa.m_variables.RutaBmp);

    if (!QFile::exists(dummy_bmp)){
        if (!m_memoryMap->m_mapa.m_variables.bPlantEnabled){
            QFileInfo info(m_memoryMap->m_mapa.m_variables.NombreRutaPdf);
            QString ext=info.suffix();
            if (ext.compare("pdf",Qt::CaseInsensitive)){
                QImage Imagenbmp;
                Imagenbmp.load(m_memoryMap->m_mapa.m_variables.NombreRutaPdf);
                if (Imagenbmp.isNull()){
                    Imagenbmp=QImage(w,h,QImage::Format_RGB32);
                    Imagenbmp.fill(0xffffffff);
                }
                Imagenbmp.save(dummy_bmp,"BMP");
                error=0;
            }
            else{
                GsRenderEngine tmpRender;
                if (m_memoryMap->m_mapa.m_variables.AlgdobleRes) tmpRender.setDoubleResolution();

                QString localFile=m_memoryMap->m_mapa.m_variables.NombreRutaPdf;
                bool buseLocalCopy=false;
                if (m_memoryMap->useLocalCopy()){ //Copiamos para hacer el render en local
                    localFile=QString("%1\\tmp\\dummyPdf.pdf").arg(QApplication::applicationDirPath());
                    if (QFile::exists(localFile)) QFile::remove(localFile);
                    QFile fooFile(m_memoryMap->m_mapa.m_variables.NombreRutaPdf);
                    fooFile.copy(localFile);
                    buseLocalCopy=true;
                    dummy_bmp=QString("%1\\tmp\\dummy.bmp").arg(QApplication::applicationDirPath());
                    if (QFile::exists(dummy_bmp)) QFile::remove(dummy_bmp);
                    //infoDebug->ProcessMsg(LOGGER_ERROR,QString("Hasta la copia %1").arg(fooTimer.elapsed()));
                }

                tmpRender.setDitheringBits(m_memoryMap->m_mapa.m_variables.iBitsDithering);
                tmpRender.setInputFile(localFile);
                tmpRender.setOutputFile(dummy_bmp);
                error=tmpRender.render();
                infoDebug->ProcessMsg(LOGGER_ERROR,tmpRender.getLastCommand());
                //infoDebug->ProcessMsg(LOGGER_ERROR,QString("Hasta el render %1").arg(fooTimer.elapsed()));
                if (buseLocalCopy) {
                    /*QFile::remove(localFile);*/
                }

            }
        }
    }

    if (error!=0) {
        QMessageBox::warning(0,"INFO",tr("Error: %1\n Error al generar imagen").arg(error),QMessageBox::Ok);
    }
    else{
        //Inicializar_posicion_colores();
        QImage *ImagenOrigen=new QImage(dummy_bmp,"BMP");

        if (w>(unsigned int)ImagenOrigen->width()) w=ImagenOrigen->width();
        if (h>(unsigned int)ImagenOrigen->height()) h=ImagenOrigen->height();
        //QImage Imagen(w,h,QImage::Format_RGB32);

        QRgb value;

        QImage *Imagen=new QImage(ImagenOrigen->copy(x0,y0,w,h).convertToFormat(QImage::Format_RGB32));
        delete (ImagenOrigen);

        for(int color=0;color<m_memoryMap->m_mapa.m_variables.NumColores;color++){
            unsigned int xpix_color=m_memoryMap->m_mapa.m_colores[color].x_pix;
            unsigned int ypix_color=m_memoryMap->m_mapa.m_colores[color].y_pix;
            if ((xpix_color!=0)||(ypix_color!=0)){
                value=Imagen->pixel(xpix_color,ypix_color);
                m_memoryMap->m_mapa.m_colores[color].valor=value;
            }
        }

        bool hayMascara=false;
        if ((m_memoryMap->m_mapa.m_ColorMascara.x_pix!=DEFAULT_NOMASK_PIXEL)&& (m_memoryMap->m_mapa.m_ColorMascara.y_pix!=DEFAULT_NOMASK_PIXEL)){
            hayMascara=true;
            unsigned int xpix_color=m_memoryMap->m_mapa.m_ColorMascara.x_pix;
            unsigned int ypix_color=m_memoryMap->m_mapa.m_ColorMascara.y_pix;
            value=Imagen->pixel(xpix_color,ypix_color);
            m_memoryMap->m_mapa.m_ColorMascara.valor=value;
        }

        if (!m_memoryMap->m_mapa.m_variables.NumColores) m_memoryMap->m_mapa.m_variables.NumColores=1;
        QVector<QImage> Imagen_CX;
        Imagen_CX.resize(m_memoryMap->m_mapa.m_variables.NumColores);
        QImage ImagenMascara(w,h,QImage::Format_RGB32);
        ImagenMascara.fill(0xffffffff);

        QVector<QRgb> vectorColor[m_memoryMap->m_mapa.m_variables.NumColores];
        for (int x=0;x<m_memoryMap->m_mapa.m_variables.NumColores;x++){
            Imagen_CX[x]=QImage(w,h,QImage::Format_RGB32);
            Imagen_CX[x].fill(0x00ffffff);
            if (!m_memoryMap->m_mapa.m_colores[x].cadena.isEmpty()){
                QStringList lst=m_memoryMap->m_mapa.m_colores[x].cadena.split("@");
                for (int y=0;y<lst.count();y++){
                    vectorColor[x].append(lst.at(y).toUInt());
                }
            }
            else{
                vectorColor[x].append(m_memoryMap->m_mapa.m_colores[x].valor);
            }

        }
        int h=Imagen->height();
        int w=Imagen->width();
        for(int y = 0; y<h; y++){
            QRgb * line = (QRgb *)Imagen->scanLine(y);            
            for(int x = 0; x<w; x++){
                value=line[x];
                if (hayMascara){
                    if ( value==m_memoryMap->m_mapa.m_ColorMascara.valor){
                        //ImagenMascara.setPixel(x,y,line[x]);
                       continue;
                    }
                    else{
                        for (int index=0;index<m_memoryMap->m_mapa.m_variables.NumColores;index++){
                            if ((line[x]==m_memoryMap->m_mapa.m_colores[index].valor)||((m_memoryMap->m_mapa.m_colores[0].x_pix==DEFAULT_NOMASK_PIXEL)&&(m_memoryMap->m_mapa.m_colores[0].y_pix==DEFAULT_NOMASK_PIXEL))) {
                                Imagen_CX[index].setPixel(x,y, value);
                            }
                        }
                    }
                }
                else{
                    for (int index=0;index<m_memoryMap->m_mapa.m_variables.NumColores;index++){
                        if ((/*value==m_memoryMap->m_mapa.m_colores[index].valor*/vectorColor[index].contains(value))||((m_memoryMap->m_mapa.m_colores[0].x_pix==DEFAULT_NOMASK_PIXEL)&&(m_memoryMap->m_mapa.m_colores[0].y_pix==DEFAULT_NOMASK_PIXEL))) {
                            Imagen_CX[index].setPixel(x,y,value);
                            //line2[x]=value;
                        }
                    }
                }
            }
        }

        QFileInfo foo(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
        QString tmpNombreRuta=(m_memoryMap->useLocalCopy())?QString("%1\\tmp\\%2").arg(QApplication::applicationDirPath()).arg(foo.fileName()):m_memoryMap->m_mapa.m_variables.NombreRutaBmp;
        QString dummyRuta=QString(/*m_memoryMap->m_mapa.m_variables.NombreRutaBmp*/tmpNombreRuta);

        Imagen->setDotsPerMeterX(14174);
        Imagen->setDotsPerMeterY(14174);
        Imagen->save(/*m_memoryMap->m_mapa.m_variables.NombreRutaBmp*/tmpNombreRuta,"BMP");
        delete (Imagen);
        if (m_memoryMap->useLocalCopy()){
            QFile file(tmpNombreRuta);
            file.copy(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
        }

        for (int x=0;x<m_memoryMap->m_mapa.m_variables.NumColores;x++){
            dummyRuta.replace(".bmp",QString("_C%1.bmp").arg(x+1));
            QString orgruta=QString(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
            orgruta.replace(".bmp",QString("_C%1.bmp").arg(x+1));

            if (QFile::exists(dummyRuta)) QFile::remove(dummyRuta);
            CMYKConfig config;



            if (config.getCalcFormat()!=QImage::Format_RGB32){
                //Posibles transformaciones
                In2CMYKGenerator *gen=new In2CMYKGenerator(Imagen_CX[x]);
                gen->getImage().save(dummyRuta,"BMP");
                if (m_memoryMap->useLocalCopy()){
                    QFile file(dummyRuta);
                    file.copy(orgruta);
                }
                delete (gen);
            }
            else{
                Imagen_CX[x].save(dummyRuta,"BMP");
                if (m_memoryMap->useLocalCopy()){
                    QFile file(dummyRuta);
                    file.copy(orgruta);
                }
            }
        }

        //Guardo la máscara
        if (hayMascara){
            QString rutaMascara=QString(/*m_memoryMap->m_mapa.m_variables.NombreRutaBmp*/tmpNombreRuta);
            rutaMascara.replace(".bmp",QString("SingleMask.bmp"));
            ///ImagenMascara.save(rutaMascara,"BMP");
        }

    }
}

/** Devuelve un  vector con todos los tonos de la imagen*/
QVector<int> In2ImageCreator::getTonos(const QImage & img){
    QVector<int> vector;
    int height=img.height();
    int width=img.width();
    for(int y = 0; y<height; y++){
        QRgb * line = (QRgb *)img.scanLine(y);
        for(int x = 0; x<width; x++){
            QRgb value=line[x];
            QColor foo(value);
            int hue=foo.hslHue();
            if (!vector.contains(hue)) vector.append(hue);
        }
    }
    return vector;
}



/** Coge y guarda una imagen individual de color del mapa de memoria*/
QImage In2ImageCreator::getColorImage(QString foo,int index){

    QImage Imagen_CX;
    QImage rtnImage;
    if (QFile::exists(foo)){
        QImage ImagenOriginal(foo);
        unsigned int x0=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.XinicialPlantilla);
        unsigned int y0=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.YinicialPlantilla);
        unsigned int w=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.AnchoPlantilla);
        unsigned int h=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.AltoPlantilla);

        if (w>(unsigned int)ImagenOriginal.width()) w=ImagenOriginal.width();
        if (h>(unsigned int)ImagenOriginal.height()) h=ImagenOriginal.height();
        QImage Imagen=ImagenOriginal.copy(x0,y0,w,h).convertToFormat(QImage::Format_RGB32);


        unsigned int xpix_color=m_memoryMap->m_mapa.m_colores[index].x_pix;
        unsigned int ypix_color=m_memoryMap->m_mapa.m_colores[index].y_pix;
        if ((xpix_color!=0)||(ypix_color!=0)){
            m_memoryMap->m_mapa.m_colores[index].valor=Imagen.pixel(xpix_color,ypix_color);
        }

        bool hayMascara=false;
        if ((m_memoryMap->m_mapa.m_ColorMascara.x_pix!=DEFAULT_NOMASK_PIXEL)&& (m_memoryMap->m_mapa.m_ColorMascara.y_pix!=DEFAULT_NOMASK_PIXEL)){
            hayMascara=true;
            unsigned int xpix_color=m_memoryMap->m_mapa.m_ColorMascara.x_pix;
            unsigned int ypix_color=m_memoryMap->m_mapa.m_ColorMascara.y_pix;
            m_memoryMap->m_mapa.m_ColorMascara.valor=Imagen.pixel(xpix_color,ypix_color);
        }

        Imagen_CX=Imagen;
#if QT_VERSION>=0x050000
        Imagen_CX.fill(Qt::white);
#else
        Imagen_CX.fill(QColor(Qt::white).rgb());
#endif
        QVector<QRgb> vectorColor;
        if (!m_memoryMap->m_mapa.m_colores[index].cadena.isEmpty()){
            QStringList lst=m_memoryMap->m_mapa.m_colores[index].cadena.split("@");
            for (int x=0;x<lst.count();x++){
                vectorColor.append(lst.at(x).toUInt());
            }
        }
        else{
            vectorColor.append(m_memoryMap->m_mapa.m_colores[index].valor);
        }

    //Este es mas rapido
        QRgb value;
        for(int y = 0; y<Imagen.height(); y++){
            QRgb * line = (QRgb *)Imagen.scanLine(y);
            for(int x = 0; x<Imagen.width(); x++){
                value=line[x];
                if (hayMascara){
                    if ( value==m_memoryMap->m_mapa.m_ColorMascara.valor){
                        continue;
                    }
                    else{
                        if ((line[x]==m_memoryMap->m_mapa.m_colores[index].valor)||((m_memoryMap->m_mapa.m_colores[index].x_pix==DEFAULT_NOMASK_PIXEL)&&(m_memoryMap->m_mapa.m_colores[index].y_pix==DEFAULT_NOMASK_PIXEL))) {
                            Imagen_CX.setPixel(x,y, value);
                        }
                    }
                }
                else{
                    if (m_memoryMap->m_mapa.m_variables.AlgSelColor!=SELECCION_AUTOMATICA){
                        if ((/*value==m_memoryMap->m_mapa.m_colores[index].valor*/vectorColor.contains(value))||((m_memoryMap->m_mapa.m_colores[index].x_pix==DEFAULT_NOMASK_PIXEL)&&(m_memoryMap->m_mapa.m_colores[index].y_pix==DEFAULT_NOMASK_PIXEL))) {
                            Imagen_CX.setPixel(x,y,value);
                        }
                    }
                }
            }
        }
        //Aplicamos las transformaciones

        /*QString alg=m_memoryMap->m_mapa.m_colores[index].alg;
        if (!alg.compare(TXT_DIFFUSE)){
            rtnImage=Imagen_CX.convertToFormat(QImage::Format_Indexed8,Qt::DiffuseDither);
        }
        else if (!alg.compare(TXT_ORDERED)) rtnImage=Imagen_CX.convertToFormat(QImage::Format_Indexed8,Qt::OrderedDither);
        else if (!alg.compare(TXT_THRESHOLD)) rtnImage=Imagen_CX.convertToFormat(QImage::Format_Indexed8,Qt::ThresholdDither);
        else if (!alg.compare(TXT_FLOYD)){
            In2FloydSteinberg fs;
            QVector<QRgb> table=fs.getOptimumTable(Imagen_CX,4);
            rtnImage=fs.convert(Imagen_CX,table);
        }
        else rtnImage=Imagen_CX;*/
        rtnImage=Imagen_CX;


        //rtnImage=QImage();
        QString dummyruta=QString(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
        dummyruta.replace(".bmp",QString("_C%1.bmp").arg(index+1));
        if (QFile::exists(dummyruta)) QFile::remove(dummyruta);
        rtnImage.save(dummyruta,"BMP");
    }
    return rtnImage;
}



/** Dibujo un item individual*/
QPixmap In2ImageCreator::DrawFixedItem(int i,int matriz){

    float Xtemp=0,Ytemp=0;
    unsigned int celda;
    QString celda_s;
    QString Vtemp;
    QStringList lista_excepciones;
    QString excepciones;
    float dix,diy;
    float incX,incY;
    bool esunaexcepcion;
    int MaxWidth=0;
    int MaxHeight=0;
    unsigned char indice_color=0;


    Vtemp=QString(m_memoryMap->m_mapa.m_campos[i].ValorActualCampo);
    int desp=0;
    if ((m_memoryMap->m_mapa.m_campos[i].m_matrices[0].PosicionYCampo)&&(m_memoryMap->m_mapa.m_campos[i].m_matrices[0].incFilas))
        desp=qAbs(m_memoryMap->m_mapa.m_campos[i].m_matrices[0].PosicionYCampo)*FACTOR_PIXELS_MM;
    QImage pic(m_memoryMap->m_Imagen.width(),m_memoryMap->m_Imagen.height()+desp,QImage::Format_ARGB32);

#if QT_VERSION >=0x050000
    pic.fill(QColor(255,255,255,0));
#else
    pic.fill(/*QColor(255,255,255,0).argb*/0x00FFFFFF);

#endif



    QPainter p(&pic);   // create a QPainter for it
    //p.fillRect(pic.rect(),Qt::transparent);
    //p.setOpacity(1);
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);




    if (m_memoryMap->m_mapa.m_variables.NumColores==1){
        p.setPen(Qt::black);
        indice_color= m_memoryMap->m_mapa.m_campos[i].color-1;
    }
    else{
        indice_color= m_memoryMap->m_mapa.m_campos[i].color-1;
        ///p.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
        p.setPen(Qt::black);
    }

    if ((m_memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01) &&
            (m_memoryMap->m_mapa.m_campos[i].color==(indice_color+1))) {
        int j=matriz;
            if (m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
                lista_excepciones.clear();
                excepciones=QString(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
                lista_excepciones=excepciones.split(",");
                dix=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo);
                diy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo);
                incX=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas);
                incY=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas);

                Vtemp=QString(m_memoryMap->m_mapa.m_campos[i].ValorActualCampo);
                QFont mifont=_setFont(i,true);
                p.setFont(mifont);
                Ytemp=0;
                celda=1;

                for (unsigned int filaMatriz=0;filaMatriz<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                    Xtemp=0;
                    unsigned int columnaMatriz=0;
                    for (columnaMatriz=0;columnaMatriz<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                        float YMod=Ytemp+incY*columnaMatriz;
                        celda_s=QString("%1").arg(celda);
                        esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                        if (esunaexcepcion==false){
                            p.save();
                            if (!m_memoryMap->m_mapa.m_campos[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                //Nos movemos menos...
                                int Orientacion=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
                                p.translate(Xtemp,YMod);
                                //p.rotate(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                struct m_BarcodeParameters bc=m_memoryMap->m_mapa.m_campos[i].m_barcodeParameters;
                                QSize size=drawBarcode(p,bc,Orientacion,Vtemp);
                                int tmpMaxWidth=Xtemp+size.width();
                                int tmpMaxHeight=YMod+size.height();
                                if (tmpMaxWidth>MaxWidth) MaxWidth=tmpMaxWidth;
                                if (tmpMaxHeight>MaxHeight) MaxHeight=tmpMaxHeight;


                            }
                            else{
                                int Orientacion=m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
                                p.translate(Xtemp,YMod);
                                QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                QPixmap dest;
                                if (Orientacion){
                                    dest=foo.transformed(QTransform().rotate(Orientacion));
                                    p.drawPixmap(0,0,dest);
                                    this->drawHoverRect(p,dest.rect(),i);
                                    int tmpMaxWidth=Xtemp+dest.width();
                                    int tmpMaxHeight=YMod+dest.height();
                                    if (tmpMaxWidth>MaxWidth) MaxWidth=tmpMaxWidth;
                                    if (tmpMaxHeight>MaxHeight) MaxHeight=tmpMaxHeight;

                                }
                                else{
                                    p.drawPixmap(0,0,foo);
                                    this->drawHoverRect(p,foo.rect(),i);
                                    int tmpMaxWidth=Xtemp+foo.width();
                                    int tmpMaxHeight=YMod+foo.height();
                                    if (tmpMaxWidth>MaxWidth) MaxWidth=tmpMaxWidth;
                                    if (tmpMaxHeight>MaxHeight) MaxHeight=tmpMaxHeight;
                                }

                            }
                             p.restore();
                        }
                        Xtemp+=dix+columnaMatriz*incX;
                        celda++;
                    }
                    Ytemp+=diy;
                }
            }
       // }
    }
    p.end();
    //Artificio para devolver solo el trozo de imagen que me interesa
    if ((MaxWidth)&&(MaxHeight)){
        QPixmap finalPixmap=QPixmap::fromImage(pic);
        return finalPixmap.copy(0,0,MaxWidth,MaxHeight+desp);

    }
    else return QPixmap();
}



/** Dibujo un item individual*/
QPixmap In2ImageCreator::DrawVariableItem(int i,int matriz){
    //unsigned char tamano_fuente_campo;
    float Xtemp,Ytemp;
    unsigned int celda;
    QString celda_s;
    QString Vtemp;
    QStringList lista_excepciones;
    QString excepciones;
    float dix,diy;
    float incX,incY;
    bool esunaexcepcion;
    //unsigned char indice_color=0;
    int MaxWidth=0;
    int MaxHeight=0;


//qDebug()<< "Dibujando";
    Xtemp=0;
    Ytemp=0;
    //tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
    Vtemp=QString(m_memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo);
    if (Vtemp.isEmpty()) Vtemp=QString("Variable%1").arg(i+1);

    //QPixmap pic(m_memoryMap->m_Imagen.size());
    QPixmap pic(m_memoryMap->m_Imagen.width(),m_memoryMap->m_Imagen.height());

    //pic.fill(/*Qt::transparent*/QColor(255,255,255,1).rgb());
    pic.fill(Qt::transparent);
    QPainter p(&pic);   // create a QPainter    QPainter p(&pic);   // create a QPainter for it
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);

    if (pic.hasAlpha()){
        p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
    }

    if (m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
        //tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo);
        QFont mifont=_setFont(i,false);
        p.setFont(mifont);
        if (m_memoryMap->m_mapa.m_variables.NumColores==1){
            p.setPen(Qt::black);
            //indice_color= m_memoryMap->m_mapa.m_camposvariables[i].color-1;
        }
        else{
            //indice_color= m_memoryMap->m_mapa.m_camposvariables[i].color-1;
            ///p.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
             p.setPen(Qt::black);
        }
        int j=matriz;
        //for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            if (m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                lista_excepciones.clear();
                excepciones=QString(m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                lista_excepciones=excepciones.split(",");
                dix=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                diy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);
                incX=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas);
                incY=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas);

                        Ytemp=0;
                        celda=1;
                        for (unsigned int filaMatriz=0;filaMatriz<m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                            Xtemp=0;
                            for (unsigned int columnaMatriz=0;columnaMatriz<m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                float YMod=Ytemp+incY*columnaMatriz;
                                celda_s=QString("%1").arg(celda);
                                esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                if (esunaexcepcion==false){
                                    p.save();
                                    if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                        Vtemp=_getCounterValue(i,filaMatriz,columnaMatriz,0);
                                    }
                                    //Si es un campo de origen BASE DE DATOS
                                    else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                        Vtemp=_getBBDDValue(i);
                                    }
                                    //Si es un campo de tipo Fecha/Hora
                                    else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                        Vtemp=_getDateTimeValue(i);
                                    }
                                    //Si es un campo de tipo Aleatorio
                                    else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                        Vtemp=_getRandomValue(i);
                                    }
                                    else{
                                        Vtemp=QString("Origen?");
                                    }
                                    if (!m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                        int Orientacion=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                        struct m_BarcodeParameters bc=m_memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                        In2BarcodeClass barcodeInst;
                                        barcodeInst.setParams(bc);
                                        barcodeInst.SetPixmap(Vtemp);
                                        p.translate(Xtemp,YMod);
                                        //p.rotate(Orientacion);
                                        QPixmap foo=this->convertToTransparent(barcodeInst.qpxImagen.transformed(QTransform().rotate(Orientacion)));
                                        //p.drawPixmap(0,0,barcodeInst.qpxImagen);
                                        p.drawPixmap(0,0,foo);
                                        drawHoverRect(p,foo.rect(),i+NUM_CAMPOS);
                                        int tmpMaxWidth=Xtemp+foo.width();
                                        int tmpMaxHeight=YMod+foo.height();
                                        if (tmpMaxWidth>MaxWidth) MaxWidth=tmpMaxWidth;
                                        if (tmpMaxHeight>MaxHeight) MaxHeight=tmpMaxHeight;


                                        //Aqui calculo el rectanculo

                                    }
                                    else {
                                        int Orientacion=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                        p.translate(Xtemp,YMod);
                                        QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                        QPixmap dest;
                                        if (Orientacion){
                                            dest=foo.transformed(QTransform().rotate(Orientacion));
                                            p.drawPixmap(0,0,dest);
                                            this->drawHoverRect(p,dest.rect(),i+NUM_CAMPOS);
                                            int tmpMaxWidth=Xtemp+dest.width();
                                            int tmpMaxHeight=YMod+dest.height();
                                            if (tmpMaxWidth>MaxWidth) MaxWidth=tmpMaxWidth;
                                            if (tmpMaxHeight>MaxHeight) MaxHeight=tmpMaxHeight;


                                        }
                                        else{
                                            p.drawPixmap(0,0,foo);
                                            this->drawHoverRect(p,foo.rect(),i+NUM_CAMPOS);
                                            int tmpMaxWidth=Xtemp+foo.width();
                                            int tmpMaxHeight=YMod+foo.height();
                                            if (tmpMaxWidth>MaxWidth) MaxWidth=tmpMaxWidth;
                                            if (tmpMaxHeight>MaxHeight) MaxHeight=tmpMaxHeight;


                                        }
                                    }
                                    p.restore();
                                }
                                Xtemp+=dix+columnaMatriz*incX;
                                celda++;
                            }
                            Ytemp+=diy;
                        }
                    }
                //}
        p.end();
    }
    //Artificio para devolver solo el trozo de imagen que estamos usando
    QPixmap finalPixmap=pic.copy(0,0,MaxWidth,MaxHeight);
    //return pic;
    return finalPixmap;
}

QPixmap In2ImageCreator::returnPixmapFromText(QPainter &p,const QString & text){
    /*QFont myFont=p.font();
    myFont.setLetterSpacing(QFont::AbsoluteSpacing,5);
    p.setFont(myFont);*/
    QFontMetrics metrics=p.fontMetrics();

    //QSize size=metrics.size(Qt::TextSingleLine,text);
    QSize size=metrics.boundingRect(text).size();
    //QSize size=metrics.boundingRect(QRect(0,0,0,0), Qt::AlignLeft|Qt::AlignBottom, text).size();
    QPixmap foo(size);
    //foo.fill(Qt::transparent);
    foo.fill(QColor(255,255,255,1));
    QPainter painter(&foo);
    //painter.fillRect(foo.rect(),Qt::transparent);
    painter.setFont(p.font());
    painter.setPen(QColor(Qt::black));
    //painter.drawText(0,size.height(),text);
    QRect rect=QRect(QPoint(0,0),size);
    painter.drawText(rect,Qt::AlignTop,text);
    painter.end();

    return foo;
}


/** Devuelve el mismo pixmap transparente*/
QPixmap In2ImageCreator::convertToTransparent(const QPixmap & q){
    QPixmap dest=QPixmap(q.size());
    dest.fill(Qt::transparent);
    QBitmap mask=q.createMaskFromColor(Qt::black,Qt::MaskOutColor);
    QPainter p(&dest);
    p.setPen(QColor(Qt::black));
    p.drawPixmap(dest.rect(),mask,mask.rect());
    p.end();
    return dest;
}

/** Dibuja el contorno*/
void In2ImageCreator::drawHoverRect(QPainter &p, QRect rect,int index){
    QPen pen;
    if (index==0) pen=QPen(Qt::blue, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else if (index==1) pen=QPen(Qt::green, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else if (index==2) pen=QPen(Qt::magenta, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else if (index==3) pen=QPen(Qt::yellow, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else if (index==4) pen=QPen(Qt::blue, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else if (index==5) pen=QPen(Qt::green, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else if (index==6) pen=QPen(Qt::magenta, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else if (index==7) pen=QPen(Qt::yellow, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    else pen=QPen(Qt::blue, 5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

    pen.setCosmetic(true);
    p.setPen(pen);
    p.drawRect(rect);
}

/** Devuelve la imagen que se le pasa con un margen a dcha e izquierda*/
QPixmap In2ImageCreator::returnImageWithMargin(const QPixmap & pixmap){
    //int margin=pixmap.width()/m_memoryMap->m_mapa.m_variables.DefaultNumHeads;
    int margin=DEFAULT_MARGIN;/*m_memoryMap->m_Imagen.width()/m_memoryMap->m_mapa.m_variables.DefaultNumHeads;*/
    QPixmap lienzo(pixmap.width()+margin*2,pixmap.height()+margin*2);
    lienzo.fill(Qt::transparent);
    QPainter p(&lienzo);
    QPen pen(Qt::red, 5, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(pen);
    p.drawPixmap(margin,margin,pixmap);
    p.end();
    return lienzo;
}

/** Dibuja un codigo de barras*/
QSize In2ImageCreator::drawBarcode(QPainter &p,struct m_BarcodeParameters bc, int Orientacion,QString text){
    In2BarcodeClass barcodeInst;
    barcodeInst.setParams(bc);
    barcodeInst.SetPixmap(text);
    QPixmap foo=barcodeInst.qpxImagen.transformed(QTransform().rotate(Orientacion));
    p.drawPixmap(0,0,foo);
    return foo.size();
}

/** Hace transformaciones*/
QImage In2ImageCreator::ditherMono(QImage image){
    return image.convertToFormat(QImage::Format_Mono,Qt::DiffuseDither);
}
/** Hace transformaciones*/
QImage In2ImageCreator::ditherMono(QString image){
    QImage foo(image);
    if (!foo.isNull()){
        return foo.convertToFormat(QImage::Format_Mono,Qt::DiffuseDither);
    }
    else{
        return QImage();
    }
}


/** Crea cada una de las imagenes que forma la impresion*/
QImage In2ImageCreator::CreaImagenVariablePorColumnasUpdated(int index,QImage::Format format,bool bHighSpeed){

    //unsigned char tamano_fuente_campo;
    float Xtemp,Ytemp;
    float dix,diy;
    float incX,incY;
    unsigned int celda;
    QString Vtemp;

    QImage PixmapPlantilla(m_memoryMap->m_Imagen.width(),m_memoryMap->m_Imagen.height(),QImage::Format_RGB32);
    QPainter p;   // create a QPainter for it
    p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
    p.begin(&PixmapPlantilla);
    p.drawPixmap(0,0,m_memoryMap->m_Imagen); // and draw your original pixmap on it

    QStringList lista_excepciones;
    //unsigned char indice_color=0;

    float Xo=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.PuntoInicialX);
    float Yo=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.PuntoInicialY);
    float sx=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.SeparacionX);
    float sy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_variables.SeparacionY);


    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==0x01){
            QFont mifont=_setFont(i,false);
            p.setFont(mifont);
            if (m_memoryMap->m_mapa.m_variables.NumColores==1){
                p.setPen(Qt::black);
                //indice_color= m_memoryMap->m_mapa.m_campos[i].color-1;
                //p.setPen(0);
            }
            else{
                //indice_color= m_memoryMap->m_mapa.m_campos[i].color-1;
                ///p.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
                p.setPen(Qt::black);

            }
            for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                if (m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==0x01){
                    lista_excepciones.clear();
                    QString excepciones=QString(m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                    lista_excepciones=excepciones.split(",");
                    float Xotemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo);
                    float Yotemp=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo);
                    dix=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo);
                    diy=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo);
                    incX=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas);
                    incY=(FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas);

                    for (unsigned int filaComposicion=0;filaComposicion<m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;filaComposicion++){
                        for (unsigned int columnaComposicion=0;columnaComposicion<m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;columnaComposicion++){
                            Ytemp=Yo+Yotemp+filaComposicion*sy;
                            celda=1;
                            for (unsigned int filaMatriz=0;filaMatriz<m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;filaMatriz++){
                                Xtemp=Xo+Xotemp+columnaComposicion*sx;
                                for (unsigned int columnaMatriz=0;columnaMatriz<m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;columnaMatriz++){
                                    float YMod=Ytemp+incY*columnaMatriz;
                                    QString celda_s=QString("%1").arg(celda);
                                    bool esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                    if (esunaexcepcion==false){
                                        p.save();
                                        if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                            Vtemp=_getCounterValue(i,filaMatriz,columnaMatriz,index);
                                        }
                                        //Si es un campo de origen BASE DE DATOS
                                        else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){
                                            Vtemp=_getBBDDValue(i);
                                        }
                                        //Si es un campo de tipo Fecha/Hora
                                        else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                                            Vtemp=_getDateTimeValue(i);
                                        }
                                        //Si es un campo de tipo Aleatorio
                                        else if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                                            Vtemp=_getRandomValue(i);
                                        }
                                        else{
                                            Vtemp=QString("Origen?");
                                        }
                                        if (!m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo.compare(TXT_CAMPO_TIPO_BARCODE)){
                                            int Orientacion=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            p.translate(Xtemp,YMod);
                                            struct m_BarcodeParameters bc=m_memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters;
                                            drawBarcode(p,bc,Orientacion,Vtemp);
                                        }
                                        else {
                                            int Orientacion=m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                                            p.translate(Xtemp,YMod);
                                            QPixmap foo=this->returnPixmapFromText(p,Vtemp);
                                            QPixmap dest;
                                            if (Orientacion){
                                                dest=foo.transformed(QTransform().rotate(Orientacion));
                                                p.drawPixmap(0,0,dest);
                                             }
                                            else{
                                                p.drawPixmap(0,0,foo);
                                            }
                                         }
                                        m_memoryMap->m_mapa.m_camposvariables[i].ValorActualCampo=Vtemp;
                                        p.restore();
                                    }
                                    Xtemp+=dix+columnaMatriz*incX;
                                    celda++;
                                }
                                Ytemp+=diy;
                            }
                          }
                        }
                      }
                    }
                }
            }
        p.end();

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
}

/** De momento devuelve la compoi para 2 heads*/
QPixmap In2ImageCreator::returnCompoImage(QString name){
    /*In2ConfiguratorDesigner config;
    QPixmap destImage(name);
    QPixmap mask;
    if (!destImage.isNull()){
        QPainter painter(&destImage);
        painter.setCompositionMode(QPainter::RasterOp_SourceOrDestination);
        //painter.setCompositionMode(QPainter::RasterOp_SourceAndDestination);
        mask=QPixmap(destImage);
        QRgb color=(QRgb)config.getColor1();
        mask.fill(color);
        painter.drawPixmap(0,0,mask);
        //painter.setCompositionMode(QPainter::CompositionMode_Multiply);
        //painter.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
        int colors=m_memoryMap->m_mapa.m_variables.NumColores;
        if (colors>1){
            for (int x=1;x<NUM_COLORES;x++){
                QString foo=name;
                foo.replace("_C1_",QString("_C%1_").arg(x+1));
                QPixmap img(foo);
                if ((!img.isNull())&&(m_memoryMap->m_mapa.m_colores[x].enabled)){
                    QRgb color=Qt::red;
                    if (x==1)   color=(QRgb)config.getColor2();
                    else  if (x==2) color=(QRgb)config.getColor3();
                    else if (x==3) color=(QRgb)config.getColor4();
                    mask.fill(color);
                    QPainter p(&img);
                    p.setCompositionMode(QPainter::RasterOp_SourceOrDestination);
                    //painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                    p.drawPixmap(0,0,mask);
                    p.end();
                    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
                    painter.drawPixmap(0,0,img);
                }
            }
        }
    }*/
    In2ConfiguratorDesigner config;
    QPixmap destImage=QPixmap();
    QPainter painter;
    QPixmap mask;
    for (int x=0;x<NUM_COLORES;x++){
        QString foo=name;
        foo.replace("_C1_",QString("_C%1_").arg(x+1));
        QPixmap img(foo);
        if ((!img.isNull())&&(m_memoryMap->m_mapa.m_colores[x].enabled)){
            QRgb color=Qt::black;
            if (x==0)   color=(QRgb)config.getColor1();
            else if (x==1)   color=(QRgb)config.getColor2();
            else  if (x==2) color=(QRgb)config.getColor3();
            else if (x==3) color=(QRgb)config.getColor4();
            mask.fill(color);
            QPainter p(&img);
            p.setCompositionMode(QPainter::RasterOp_SourceOrDestination);
            //painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            p.drawPixmap(0,0,mask);
            p.end();
            if (destImage.isNull()){
                /*destImage=img;
                painter.begin(&destImage);*/
                destImage=QPixmap(foo);
                painter.begin(&destImage);
                painter.setCompositionMode(QPainter::RasterOp_SourceOrDestination);
                mask=QPixmap(destImage);
                mask.fill(color);
                painter.drawPixmap(0,0,mask);

            }
            else{
                painter.setCompositionMode(QPainter::CompositionMode_Multiply);
                painter.drawPixmap(0,0,img);
           }

        }
    }

    if (destImage.isNull()){
        QString foo=name;
        foo.replace("_C1_",QString("_C1_"));
        destImage=QPixmap(foo);

    }

    return destImage;
    //return QPixmap();
}

/** De momento devuelve la compo de 2 imagenes*/
QPixmap In2ImageCreator::returnCompoImage(QString name1,QString name2){
    QPixmap lienzo(name1);
    QPixmap pixmapHead2(name2);
    QPixmap MaskPixmap(pixmapHead2.size());
    if ((!lienzo.isNull())&&(!pixmapHead2.isNull())){
        MaskPixmap.fill(Qt::red);
        QPainter p(&pixmapHead2);
        p.setCompositionMode(QPainter::RasterOp_SourceOrDestination);
        p.drawPixmap(0,0,MaskPixmap);
        p.end();
        p.begin(&lienzo);
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        p.drawPixmap(0,0,pixmapHead2);
        p.end();
        return lienzo;
    }
    else if ((!lienzo.isNull())&&(pixmapHead2.isNull())) return lienzo;
    else if ((lienzo.isNull())&&(!pixmapHead2.isNull())) return pixmapHead2;
    else return QPixmap();

}



/** De momento devuelve la compoi para 2 heads*/
QPixmap In2ImageCreator::returnCompoImageExt(QString name){
    QString OriginalName=name;
    QPixmap lienzo;
    if (OriginalName.contains("_C1.bmp")){
        QString ImageHead2=OriginalName.remove("_C1.bmp");
        ImageHead2.append("_C2.bmp");
        QFile testFile(ImageHead2);
        if (testFile.exists()){
            lienzo=QPixmap(name);
            QPainter p(&lienzo);
     //       p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
//            p.drawPixmap(0,0,QPixmap(ImageHead2));
            p.end();
        }
        else lienzo=QPixmap(name);
    }
    return lienzo;
}


