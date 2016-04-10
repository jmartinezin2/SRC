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
#include "in2compofile.h"
#include <QSettings>
#include "in2defines.h"
#include "MemoryMap.h"
#include "in2hash.h"
#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>

In2CompoFile::In2CompoFile()
{
    memoryMap=MemoryMap::request(0);
    txt_CAMPO.clear();
    for(unsigned char i=0;i<NUM_CAMPOS;i++)  txt_CAMPO.append(QString("%1_%2").arg(TXT_CAMPO).arg(i+1));

    txt_CAMPO_VARIABLE.clear();
    for(unsigned char i=0;i<NUM_CAMPOS_VARIABLES;i++) txt_CAMPO_VARIABLE.append(QString("%1_%2").arg(TXT_CAMPO_VARIABLE).arg(i+1));

}

/** Guarda fichero de composición*/
void In2CompoFile::GuardarFicheroIni(){
    QString dummy_string;
    int dummy_int;
    double dummy_double;
    bool dummy_bool;

    dummy_string=QString(memoryMap->m_mapa.m_variables.NombreRutaIni);
    QSettings FicheroIni(dummy_string,QSettings::IniFormat,0);

    FicheroIni.beginGroup(TXT_PROPIEDADES);
    FicheroIni.setValue(TXT_CREADOPOR,memoryMap->m_mapa.m_variables.CreadoPor);
    FicheroIni.setValue(TXT_CREADOCUANDO,memoryMap->m_mapa.m_variables.CreadoCuando);
    FicheroIni.setValue(TXT_MODIFICADOPOR,memoryMap->m_mapa.m_variables.ModificadoPor);
    FicheroIni.setValue(TXT_MODIFICADOCUANDO,memoryMap->m_mapa.m_variables.ModificadoCuando);
    FicheroIni.endGroup();



    FicheroIni.beginGroup(TXT_IMAGEN_FONDO);

    FicheroIni.setValue(TXT_PLANTENABLED,memoryMap->m_mapa.m_variables.bPlantEnabled);
    if (memoryMap->m_mapa.m_variables.bPlantEnabled){
        FicheroIni.setValue(TXT_PLANTWIDTH,memoryMap->m_mapa.m_variables.fPlantAncho);
        FicheroIni.setValue(TXT_PLANTHEIGHT,memoryMap->m_mapa.m_variables.fPlantAlto);
        FicheroIni.setValue(TXT_PLANTOFFSETX,memoryMap->m_mapa.m_variables.fPlantOffsetX);
        FicheroIni.setValue(TXT_PLANTOFFSETY,memoryMap->m_mapa.m_variables.fPlantOffsetY);
        FicheroIni.setValue(TXT_PLANTDISTCOLS,memoryMap->m_mapa.m_variables.fPlantDistCols);
        FicheroIni.setValue(TXT_PLANTCOLS,memoryMap->m_mapa.m_variables.iPlantCols);
        for (int x=0;x<memoryMap->m_mapa.m_variables.iPlantCols;x++){
            QString file=QString("%1%2").arg(TXT_PLANTFILE).arg(x);
            if (x<memoryMap->m_mapa.m_variables.strPlantImages.count())
                FicheroIni.setValue(file,memoryMap->m_mapa.m_variables.strPlantImages.at(x));
            else
                FicheroIni.setValue(file,"");
        }
        //Limpiamos la zona de imagen fija
        FicheroIni.setValue(TXT_NOMBRE_PDF,"");
        FicheroIni.setValue(TXT_RUTA_PDF,"");
        FicheroIni.setValue(TXT_X_INICIAL_IMAGEN_FONDO,0);
        FicheroIni.setValue(TXT_Y_INICIAL_IMAGEN_FONDO,0);
        FicheroIni.setValue(TXT_ANCHO_IMAGEN_FONDO,0);
        FicheroIni.setValue(TXT_ALTO_IMAGEN_FONDO,0);

    }
    else{
        dummy_string=QString(memoryMap->m_mapa.m_variables.NombrePdf);
        FicheroIni.setValue(TXT_NOMBRE_PDF,dummy_string);
        dummy_string=QString(memoryMap->m_mapa.m_variables.RutaPdf);
        FicheroIni.setValue(TXT_RUTA_PDF,dummy_string);
        dummy_double=memoryMap->m_mapa.m_variables.XinicialPlantilla;
        FicheroIni.setValue(TXT_X_INICIAL_IMAGEN_FONDO,dummy_double);
        dummy_double=memoryMap->m_mapa.m_variables.YinicialPlantilla;
        FicheroIni.setValue(TXT_Y_INICIAL_IMAGEN_FONDO,dummy_double);
        dummy_double=memoryMap->m_mapa.m_variables.AnchoPlantilla;
        FicheroIni.setValue(TXT_ANCHO_IMAGEN_FONDO,dummy_double);
        dummy_double=memoryMap->m_mapa.m_variables.AltoPlantilla;
        FicheroIni.setValue(TXT_ALTO_IMAGEN_FONDO,dummy_double);
        dummy_int=(memoryMap->m_mapa.m_variables.AlgdobleRes)?1:0;
        FicheroIni.setValue(TXT_DOBLE_RES,dummy_int);
        dummy_int=memoryMap->m_mapa.m_variables.iBitsDithering;
        FicheroIni.setValue(TXT_BITS_DITH,dummy_int);
        dummy_int=(memoryMap->m_mapa.m_variables.bDilateAlgorithm)?1:0;
        FicheroIni.setValue(TXT_DILATE_ALG,dummy_int);
        dummy_int=(memoryMap->m_mapa.m_variables.iSabreAlgorithm);
        FicheroIni.setValue(TXT_SABRE_ALG,dummy_int);

    }
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_DB);
    FicheroIni.setValue(TXT_DB_NOMBRE,memoryMap->m_mapa.m_database.NombreDb);
    FicheroIni.setValue(TXT_DB_RUTA,memoryMap->m_mapa.m_database.RutaDb);
    FicheroIni.setValue(TXT_DB_TABLA_DATOS,memoryMap->m_mapa.m_database.NombreTablaDatosImprimir);
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_MAQUINA);

    FicheroIni.setValue(TXT_VELOCIDAD,memoryMap->m_mapa.m_impresion.iVelocidad);
    FicheroIni.setValue(TXT_PAL1,memoryMap->m_mapa.m_impresion.qstrPal1);
    FicheroIni.setValue(TXT_PAL2,memoryMap->m_mapa.m_impresion.qstrPal2);
    FicheroIni.setValue(TXT_PAL3,memoryMap->m_mapa.m_impresion.qstrPal3);
    FicheroIni.setValue(TXT_PAL4,memoryMap->m_mapa.m_impresion.qstrPal4);


    FicheroIni.endGroup();


    //Marca superior
    FicheroIni.beginGroup(TXT_MARCASUP);
    FicheroIni.setValue(TXT_ANGULO_COMPOSICION,QString("%1").arg(memoryMap->m_mapa.m_Marca.anguloSup));
    FicheroIni.setValue(TXT_FRECUENCIA_MACULA,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaSupFrecuencia));
    FicheroIni.setValue(TXT_ON,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaSupOnOff));
    FicheroIni.setValue(TXT_REPETICIONES_MACULA,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaSupRepeticiones));
    FicheroIni.setValue(TXT_TIPO_CAMPO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaSupTipo));
    FicheroIni.setValue(TXT_VALOR_CAMPO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaSupValor));
    FicheroIni.setValue(TXT_X_INICIAL_IMAGEN_FONDO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaSupX));
    FicheroIni.setValue(TXT_Y_INICIAL_IMAGEN_FONDO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaSupY));
    struct m_BarcodeParameters bc=memoryMap->m_mapa.m_Marca.barcodeSup;
    FicheroIni.setValue(TXT_WIDTH_BARCODE_MM,bc.width_mm);
    FicheroIni.setValue(TXT_HEIGHT_BARCODE_MM,bc.height_mm);
    FicheroIni.setValue(TXT_WIDTH_BARCODE,bc.width);
    FicheroIni.setValue(TXT_HEIGHT_BARCODE,bc.height);

    FicheroIni.setValue(TXT_CODIFICACION_BARCODE,bc.codificacion);
    FicheroIni.setValue(TXT_TEXT_ALIGNMENT,bc.textAlignement);
    FicheroIni.setValue(TXT_OPTION2_BARCODE,bc.option2);
    FicheroIni.setValue(TXT_INPUTMODE_BARCODE,bc.inputMode);
    FicheroIni.setValue(TXT_FGCOLOR_BARCODE,bc.indiceFgColor);
    FicheroIni.setValue(TXT_BGCOLOR_BARCODE,bc.indiceBgColor);
    FicheroIni.setValue(TXT_BORDERTYPE_BARCODE,bc.borderType);
    FicheroIni.setValue(TXT_BORDERWIDTH_BARCODE,bc.borderWidth);
    FicheroIni.setValue(TXT_WHITESPACE_BARCODE,bc.whitespace);
    FicheroIni.setValue(TXT_SCALE_BARCODE,bc.escala);
    FicheroIni.setValue(TXT_ASPECTRATIO_BARCODE,bc.ar);
    bc.hideText?FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"Si"):FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"No");
    FicheroIni.setValue(TXT_FONTLARGE_BARCODE,bc.fontSizeLarge);
    FicheroIni.setValue(TXT_FONTSMALL_BARCODE,bc.fontSizeSmall);
    FicheroIni.setValue(TXT_FONTTYPE_BARCODE,bc.fontType);
    FicheroIni.setValue(TXT_SEGURIDAD_BARCODE,bc.nivelSeguridad);
    FicheroIni.endGroup();

    //Marca inferior
    FicheroIni.beginGroup(TXT_MARCAINF);
    FicheroIni.setValue(TXT_ANGULO_COMPOSICION,QString("%1").arg(memoryMap->m_mapa.m_Marca.anguloInf));
    FicheroIni.setValue(TXT_FRECUENCIA_MACULA,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaInfFrecuencia));
    FicheroIni.setValue(TXT_ON,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaInfOnOff));
    FicheroIni.setValue(TXT_REPETICIONES_MACULA,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaInfRepeticiones));
    FicheroIni.setValue(TXT_TIPO_CAMPO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaInfTipo));
    FicheroIni.setValue(TXT_VALOR_CAMPO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaInfValor));
    FicheroIni.setValue(TXT_X_INICIAL_IMAGEN_FONDO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaInfX));
    FicheroIni.setValue(TXT_Y_INICIAL_IMAGEN_FONDO,QString("%1").arg(memoryMap->m_mapa.m_Marca.MarcaInfY));
    bc=memoryMap->m_mapa.m_Marca.barcodeInf;
    FicheroIni.setValue(TXT_WIDTH_BARCODE_MM,bc.width_mm);
    FicheroIni.setValue(TXT_HEIGHT_BARCODE_MM,bc.height_mm);
    FicheroIni.setValue(TXT_WIDTH_BARCODE,bc.width);
    FicheroIni.setValue(TXT_HEIGHT_BARCODE,bc.height);

    FicheroIni.setValue(TXT_CODIFICACION_BARCODE,bc.codificacion);
    FicheroIni.setValue(TXT_TEXT_ALIGNMENT,bc.textAlignement);
    FicheroIni.setValue(TXT_OPTION2_BARCODE,bc.option2);
    FicheroIni.setValue(TXT_INPUTMODE_BARCODE,bc.inputMode);
    FicheroIni.setValue(TXT_FGCOLOR_BARCODE,bc.indiceFgColor);
    FicheroIni.setValue(TXT_BGCOLOR_BARCODE,bc.indiceBgColor);
    FicheroIni.setValue(TXT_BORDERTYPE_BARCODE,bc.borderType);
    FicheroIni.setValue(TXT_BORDERWIDTH_BARCODE,bc.borderWidth);
    FicheroIni.setValue(TXT_WHITESPACE_BARCODE,bc.whitespace);
    FicheroIni.setValue(TXT_SCALE_BARCODE,bc.escala);
    FicheroIni.setValue(TXT_ASPECTRATIO_BARCODE,bc.ar);
    bc.hideText?FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"Si"):FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"No");
    FicheroIni.setValue(TXT_FONTLARGE_BARCODE,bc.fontSizeLarge);
    FicheroIni.setValue(TXT_FONTSMALL_BARCODE,bc.fontSizeSmall);
    FicheroIni.setValue(TXT_FONTTYPE_BARCODE,bc.fontType);
    FicheroIni.setValue(TXT_SEGURIDAD_BARCODE,bc.nivelSeguridad);
    FicheroIni.endGroup();





    QString dummy_txt;
    QString dummy_matriz;



    for (int i=0;i<NUM_CAMPOS;i++){
        dummy_txt=txt_CAMPO.at(i);
        FicheroIni.beginGroup(dummy_txt);
        dummy_string=QString(memoryMap->m_mapa.m_campos[i].NombreCampo);
        FicheroIni.setValue(TXT_NOMBRE_CAMPO,dummy_string);

        if (memoryMap->m_mapa.m_campos[i].OnOffCampo==1) FicheroIni.setValue(TXT_ESTADO_CAMPO,TXT_ON);
        else FicheroIni.setValue(TXT_ESTADO_CAMPO,TXT_OFF);
        dummy_string=QString(memoryMap->m_mapa.m_campos[i].TipoLetraCampo);
        FicheroIni.setValue(TXT_FUENTE_CAMPO,dummy_string);
        dummy_int=memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo;
        FicheroIni.setValue(TXT_TAMANO_CAMPO,dummy_int);
        dummy_int=memoryMap->m_mapa.m_campos[i].letterSpace;
        FicheroIni.setValue(TXT_LETTER_SPACE,dummy_int);
        dummy_txt=memoryMap->m_mapa.m_campos[i].ValorActualCampo;
        FicheroIni.setValue(TXT_VALOR_CAMPO,dummy_txt);


        dummy_string=QString(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo);       
        FicheroIni.setValue(TXT_ESTILO_CAMPO,dummy_string);

        dummy_string=QString(memoryMap->m_mapa.m_campos[i].TipoCampo);
        FicheroIni.setValue(TXT_TIPO_CAMPO,dummy_string);

        //Pongo siempre uh formato
        dummy_string=memoryMap->m_mapa.m_campos[i].m_dateTimeParameters.formato;
        FicheroIni.setValue(TXT_DATETIME_FORMATO,dummy_string);

        dummy_string=memoryMap->m_mapa.m_campos[i].TipoCampo;
        if (!dummy_string.compare(TXT_CAMPO_TIPO_BARCODE)){

            dummy_int=memoryMap->m_mapa.m_campos[i].NumCaracteresCampo;
            FicheroIni.setValue(TXT_NUM_CARACTERES_CAMPO,dummy_int);

            dummy_double=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.width_mm;
            FicheroIni.setValue(TXT_WIDTH_BARCODE,dummy_double);

            dummy_double=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.height_mm;
            FicheroIni.setValue(TXT_HEIGHT_BARCODE,dummy_double);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.codificacion;
            FicheroIni.setValue(TXT_CODIFICACION_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.textAlignement;
            FicheroIni.setValue(TXT_TEXT_ALIGNMENT,dummy_int);



            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.option2;
            FicheroIni.setValue(TXT_OPTION2_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.inputMode;
            FicheroIni.setValue(TXT_INPUTMODE_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.indiceFgColor;
            FicheroIni.setValue(TXT_FGCOLOR_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.indiceBgColor;
            FicheroIni.setValue(TXT_BGCOLOR_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.borderType;
            FicheroIni.setValue(TXT_BORDERTYPE_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.borderWidth;
            FicheroIni.setValue(TXT_BORDERWIDTH_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.whitespace;
            FicheroIni.setValue(TXT_WHITESPACE_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.escala;
            FicheroIni.setValue(TXT_SCALE_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.ar;
            FicheroIni.setValue(TXT_ASPECTRATIO_BARCODE,dummy_int);

            dummy_bool=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.hideText;
            if (dummy_bool) FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"Si");
            else FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"No");

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.fontSizeLarge;
            FicheroIni.setValue(TXT_FONTLARGE_BARCODE,dummy_int);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.fontSizeSmall;
            FicheroIni.setValue(TXT_FONTSMALL_BARCODE,dummy_int);

            dummy_string=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.fontType;
            FicheroIni.setValue(TXT_FONTTYPE_BARCODE,dummy_string);

            dummy_int=memoryMap->m_mapa.m_campos[i].m_barcodeParameters.nivelSeguridad;
            FicheroIni.setValue(TXT_SEGURIDAD_BARCODE,dummy_int);
        }


        if (memoryMap->m_mapa.m_variables.NumColores>1){
            dummy_int=memoryMap->m_mapa.m_campos[i].color;
            FicheroIni.setValue(TXT_COLOR_CAMPO,dummy_int);
        }


        for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_ESTADO_CAMPO);
            if (memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==1) FicheroIni.setValue(dummy_matriz,TXT_ON);
            else FicheroIni.setValue(dummy_matriz,TXT_OFF);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_X_CAMPO);
            dummy_double=memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo;
            FicheroIni.setValue(dummy_matriz,dummy_double);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_Y_CAMPO);
            dummy_double=memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo;
            FicheroIni.setValue(dummy_matriz,dummy_double);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_ORIENTACION_CAMPO);
            dummy_int=memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo;
            FicheroIni.setValue(dummy_matriz,dummy_int);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_FILAS_MATRIZ_CAMPO);
            dummy_int=memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;
            FicheroIni.setValue(dummy_matriz,dummy_int);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_COLUMNAS_MATRIZ_CAMPO);
            dummy_int=memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;
            FicheroIni.setValue(dummy_matriz,dummy_int);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_FILAS_MATRIZ_CAMPO);
            dummy_double=memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo;
            FicheroIni.setValue(dummy_matriz,dummy_double);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO);
            dummy_double=memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo;
            FicheroIni.setValue(dummy_matriz,dummy_double);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_INC_COLUMNAS_MATRIZ_CAMPO);
            dummy_double=memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas;
            FicheroIni.setValue(dummy_matriz,dummy_double);

            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_INC_FILAS_MATRIZ_CAMPO);
            dummy_double=memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas;
            FicheroIni.setValue(dummy_matriz,dummy_double);



            dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_EXCEPCIONES_MATRIZ_CAMPO);
            dummy_string=QString(memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
            FicheroIni.setValue(dummy_matriz,dummy_string);
        }

        FicheroIni.endGroup();
    }

    //Ahora guardo los datos variables
    if (memoryMap->ImagenVariable==0x01){
        for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
            dummy_txt=txt_CAMPO_VARIABLE.at(i);
            FicheroIni.beginGroup(dummy_txt);
            dummy_string=QString(memoryMap->m_mapa.m_camposvariables[i].NombreCampo);
            FicheroIni.setValue(TXT_NOMBRE_CAMPO,dummy_string);

            if (memoryMap->m_mapa.m_camposvariables[i].OnOffCampo==1) FicheroIni.setValue(TXT_ESTADO_CAMPO,TXT_ON);
            else FicheroIni.setValue(TXT_ESTADO_CAMPO,TXT_OFF);
            dummy_string=QString(memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo);
            FicheroIni.setValue(TXT_FUENTE_CAMPO,dummy_string);
            dummy_int=memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo;
            FicheroIni.setValue(TXT_TAMANO_CAMPO,dummy_int);
            dummy_int=memoryMap->m_mapa.m_camposvariables[i].letterSpace;
            FicheroIni.setValue(TXT_LETTER_SPACE,dummy_int);
            dummy_string=QString(memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo);
            FicheroIni.setValue(TXT_ESTILO_CAMPO,dummy_string);

            dummy_string=QString(memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo);
            //qDebug() << "Origen valor: " << dummy_string;
            FicheroIni.setValue(TXT_ORIGEN_VALOR_CAMPO,dummy_string);

            dummy_string=QString(memoryMap->m_mapa.m_camposvariables[i].TipoCampo);
            FicheroIni.setValue(TXT_TIPO_CAMPO,dummy_string);

            if  (!dummy_string.compare(TXT_CAMPO_TIPO_BARCODE)){

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].NumCaracteresCampo;
                FicheroIni.setValue(TXT_NUM_CARACTERES_CAMPO,dummy_int);

                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.width_mm;
                FicheroIni.setValue(TXT_WIDTH_BARCODE,dummy_double);

                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.height_mm;
                FicheroIni.setValue(TXT_HEIGHT_BARCODE,dummy_double);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.codificacion;
                FicheroIni.setValue(TXT_CODIFICACION_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.textAlignement;
                FicheroIni.setValue(TXT_TEXT_ALIGNMENT,dummy_int);


                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.option2;
                FicheroIni.setValue(TXT_OPTION2_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.inputMode;
                FicheroIni.setValue(TXT_INPUTMODE_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.indiceFgColor;
                FicheroIni.setValue(TXT_FGCOLOR_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.indiceBgColor;
                FicheroIni.setValue(TXT_BGCOLOR_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.borderType;
                FicheroIni.setValue(TXT_BORDERTYPE_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.borderWidth;
                FicheroIni.setValue(TXT_BORDERWIDTH_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.whitespace;
                FicheroIni.setValue(TXT_WHITESPACE_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.escala;
                FicheroIni.setValue(TXT_SCALE_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.ar;
                FicheroIni.setValue(TXT_ASPECTRATIO_BARCODE,dummy_int);

                dummy_bool=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.hideText;
                if (dummy_bool) FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"Si");
                else FicheroIni.setValue(TXT_ESCONDERTEXTO_BARCODE,"No");

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.fontSizeLarge;
                FicheroIni.setValue(TXT_FONTLARGE_BARCODE,dummy_int);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.fontSizeSmall;
                FicheroIni.setValue(TXT_FONTSMALL_BARCODE,dummy_int);

                dummy_string=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.fontType;
                FicheroIni.setValue(TXT_FONTTYPE_BARCODE,dummy_string);

                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.nivelSeguridad;
                FicheroIni.setValue(TXT_SEGURIDAD_BARCODE,dummy_int);
            }

            if (memoryMap->m_mapa.m_variables.NumColores>1){
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].color;
                FicheroIni.setValue(TXT_COLOR_CAMPO,dummy_int);
            }

            //Caso de que el campo seade tipo contador
            if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras;
                FicheroIni.setValue(TXT_CONTADOR_NUMERO_CIFRAS,dummy_int);
                quint64 dummy_64=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.limiteInferior;
                FicheroIni.setValue(TXT_CONTADOR_LIMITE_INFERIOR,dummy_64);
                dummy_64=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.limiteSuperior;
                FicheroIni.setValue(TXT_CONTADOR_LIMITE_SUPERIOR,dummy_64);
                dummy_64=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial;
                FicheroIni.setValue(TXT_CONTADOR_VALOR_INICIAL,dummy_64);
                dummy_64=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.incremento;
                FicheroIni.setValue(TXT_CONTADOR_INCREMENTO,dummy_64);
                dummy_string=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.configuracion;
                FicheroIni.setValue(TXT_CONTADOR_CONFIGURACION,dummy_string);
                dummy_64=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;
                FicheroIni.setValue(TXT_CONTADOR_OFFSET,dummy_64);
                //Metemos los valores de formato
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.numeroCifras;
                FicheroIni.setValue(TXT_CONTADOR_NUMERO_CIFRAS,dummy_int);
                dummy_bool=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.zeroPadding;
                FicheroIni.setValue(TXT_CONTADOR_ZPADDING,dummy_bool);
                dummy_bool=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.bFormatEnabled;
                FicheroIni.setValue(TXT_CONTADOR_FORMATENABLED,dummy_bool);
                dummy_int=(int)memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.FormatSeparador;
                FicheroIni.setValue(TXT_CONTADOR_FORMATLANGUAJE,dummy_int);


            }
            else  if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_DB)){

            }
            //Caso de que el campo seade tipo Fecha/Hota
            else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
                dummy_string=memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato;
                FicheroIni.setValue(TXT_DATETIME_FORMATO,dummy_string);
            }
            //Caso de que el campo seade tipo aleatorio
            else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras;
                FicheroIni.setValue(TXT_RANDOM_NUMERO_CIFRAS,dummy_int);
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior;
                FicheroIni.setValue(TXT_RANDOM_LIMITE_INFERIOR,dummy_int);
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior;
                FicheroIni.setValue(TXT_RANDOM_LIMITE_SUPERIOR,dummy_int);
            }

            for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_ESTADO_CAMPO);
                if (memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz==1) FicheroIni.setValue(dummy_matriz,TXT_ON);
                else FicheroIni.setValue(dummy_matriz,TXT_OFF);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_X_CAMPO);
                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo;
                FicheroIni.setValue(dummy_matriz,dummy_double);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_Y_CAMPO);
                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo;
                FicheroIni.setValue(dummy_matriz,dummy_double);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_ORIENTACION_CAMPO);
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo;
                FicheroIni.setValue(dummy_matriz,dummy_int);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_FILAS_MATRIZ_CAMPO);
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;
                FicheroIni.setValue(dummy_matriz,dummy_int);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_COLUMNAS_MATRIZ_CAMPO);
                dummy_int=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;
                FicheroIni.setValue(dummy_matriz,dummy_int);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_FILAS_MATRIZ_CAMPO);
                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo;
                FicheroIni.setValue(dummy_matriz,dummy_double);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO);
                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo;
                FicheroIni.setValue(dummy_matriz,dummy_double);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_INC_COLUMNAS_MATRIZ_CAMPO);
                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas;
                FicheroIni.setValue(dummy_matriz,dummy_double);

                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_INC_FILAS_MATRIZ_CAMPO);
                dummy_double=memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas;
                FicheroIni.setValue(dummy_matriz,dummy_double);


                dummy_matriz=QString("M%1_%2").arg(j+1).arg(TXT_EXCEPCIONES_MATRIZ_CAMPO);
                dummy_string=QString(memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo);
                FicheroIni.setValue(dummy_matriz,dummy_string);
            }

            FicheroIni.endGroup();
        }
    }

    FicheroIni.beginGroup(TXT_ORIGEN);
    dummy_double=memoryMap->m_mapa.m_variables.PuntoInicialX;
    FicheroIni.setValue(TXT_COORDENADA_X_ORIGEN,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.PuntoInicialY;
    FicheroIni.setValue(TXT_COORDENADA_Y_ORIGEN,dummy_double);
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_COMPOSICION);
    dummy_double=memoryMap->m_mapa.m_variables.WebWidth;
    FicheroIni.setValue(TXT_WEBWIDTH_COMPOSICION,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.WebHeight;
    FicheroIni.setValue(TXT_WEBHEIGHT_COMPOSICION,dummy_double);
    dummy_int=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
    FicheroIni.setValue(TXT_FILAS_COMPOSICION,dummy_int);
    dummy_int=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    FicheroIni.setValue(TXT_COLUMNAS_COMPOSICION,dummy_int);
    dummy_double=memoryMap->m_mapa.m_variables.SeparacionY;
    FicheroIni.setValue(TXT_SEPARACION_FILAS_COMPOSICION,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.SeparacionX;
    FicheroIni.setValue(TXT_SEPARACION_COLUMNAS_COMPOSICION,dummy_double);
    FicheroIni.setValue(TXT_ANGULO_COMPOSICION,memoryMap->m_mapa.m_variables.AnguloPlantilla);
    FicheroIni.setValue(TXT_COMENTARIO_COMPOSICION,memoryMap->m_mapa.m_variables.Comentario);
    FicheroIni.endGroup();


    FicheroIni.beginGroup(TXT_MACULA_IZQDA);
    if (memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff==1) FicheroIni.setValue(TXT_ESTADO_MACULA,TXT_ON);
    else FicheroIni.setValue(TXT_ESTADO_MACULA,TXT_OFF);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaIzqdaX;
    FicheroIni.setValue(TXT_COORDENADA_X_MACULA,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaIzqdaY;
    FicheroIni.setValue(TXT_COORDENADA_Y_MACULA,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaIzqdaAncho;
    FicheroIni.setValue(TXT_ANCHO_MACULA,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaIzqdaAlto;
    FicheroIni.setValue(TXT_ALTO_MACULA,dummy_double);
    dummy_int=memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones;
    FicheroIni.setValue(TXT_REPETICIONES_MACULA,dummy_int);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia;
    FicheroIni.setValue(TXT_FRECUENCIA_MACULA,dummy_double);
    if (memoryMap->m_mapa.m_variables.NumColores>1){
        dummy_int=memoryMap->m_mapa.m_variables.MaculaIzqdaColor;
        FicheroIni.setValue(TXT_COLOR_MACULA,dummy_int);
    }
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_MACULA_DCHA);
    if (memoryMap->m_mapa.m_variables.MaculaDchaOnOff==1) FicheroIni.setValue(TXT_ESTADO_MACULA,TXT_ON);
    else FicheroIni.setValue(TXT_ESTADO_MACULA,TXT_OFF);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaDchaX;
    FicheroIni.setValue(TXT_COORDENADA_X_MACULA,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaDchaY;
    FicheroIni.setValue(TXT_COORDENADA_Y_MACULA,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaDchaAncho;
    FicheroIni.setValue(TXT_ANCHO_MACULA,dummy_double);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaDchaAlto;
    FicheroIni.setValue(TXT_ALTO_MACULA,dummy_double);
    dummy_int=memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones;
    FicheroIni.setValue(TXT_REPETICIONES_MACULA,dummy_int);
    dummy_double=memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia;
    FicheroIni.setValue(TXT_FRECUENCIA_MACULA,dummy_double);
    if (memoryMap->m_mapa.m_variables.NumColores>1){
        dummy_int=memoryMap->m_mapa.m_variables.MaculaDchaColor;
        FicheroIni.setValue(TXT_COLOR_MACULA,dummy_int);
    }
    FicheroIni.endGroup();

    //if (memoryMap->m_mapa.m_variables.NumColores>1){
        for (unsigned char i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
            FicheroIni.beginGroup(QString("%1%2").arg(TXT_COLOR).arg(i+1));
            dummy_int=memoryMap->m_mapa.m_colores[i].x_pix;
            FicheroIni.setValue(TXT_COORDENADA_X_COLOR,dummy_int);
            dummy_int=memoryMap->m_mapa.m_colores[i].y_pix;
            FicheroIni.setValue(TXT_COORDENADA_Y_COLOR,dummy_int);
            FicheroIni.setValue(TXT_CADENA_COLOR,memoryMap->m_mapa.m_colores[i].cadena);
            FicheroIni.setValue(TXT_ENABLED,memoryMap->m_mapa.m_colores[i].enabled);
            FicheroIni.setValue(TXT_ALG_DITH,memoryMap->m_mapa.m_colores[i].alg);
            FicheroIni.setValue(TXT_ALG_WHITE,memoryMap->m_mapa.m_colores[i].bWhiteCorr);
            FicheroIni.setValue(TXT_ALG_BLACK,memoryMap->m_mapa.m_colores[i].bBlackCorr);
            FicheroIni.setValue(TXT_ALG_MONO,memoryMap->m_mapa.m_colores[i].bMonoMode);
            FicheroIni.setValue(TXT_ALG_CORRDIN,memoryMap->m_mapa.m_colores[i].bCorrDinamica);
            FicheroIni.setValue(TXT_ALG_CORRMONO,memoryMap->m_mapa.m_colores[i].bMonoCorr);
            FicheroIni.setValue(TXT_COLOR_INDEX,memoryMap->m_mapa.m_colores[i].index);
            FicheroIni.setValue(TXT_COLOR_DITHERCUT,memoryMap->m_mapa.m_colores[i].m_diherCut);
            FicheroIni.setValue(TXT_COLOR_DITHERPAL,memoryMap->m_mapa.m_colores[i].m_ditherPal);
            FicheroIni.setValue(TXT_COLOR_CUSTOMPAL,memoryMap->m_mapa.m_colores[i].m_customPal);
            FicheroIni.endGroup();
   //     }

        FicheroIni.beginGroup(TXT_COLOR);
        dummy_string=QString(memoryMap->m_mapa.m_variables.TipoColor);
        FicheroIni.setValue(TXT_TIPO_COLOR,dummy_string);
        dummy_int=memoryMap->m_mapa.m_variables.NumColores;
        FicheroIni.setValue(TXT_VALOR_COLOR,dummy_int);
        dummy_int=memoryMap->m_mapa.m_variables.AlgSelColor;
        FicheroIni.setValue(TXT_ALG_SELECCION_COLOR,dummy_int);
        dummy_int=memoryMap->m_mapa.m_variables.iGrayTh;
        FicheroIni.setValue(TXT_GRAY_THRESHOLD,dummy_int);
        FicheroIni.endGroup();
    }
    //Se guarda el color de mascara
    FicheroIni.beginGroup(TXT_MASCARA_COLOR);
    dummy_int=memoryMap->m_mapa.m_ColorMascara.x_pix;
    FicheroIni.setValue(TXT_COORDENADA_X_COLOR,dummy_int);
    dummy_int=memoryMap->m_mapa.m_ColorMascara.y_pix;
    FicheroIni.setValue(TXT_COORDENADA_Y_COLOR,dummy_int);
    FicheroIni.endGroup();
    SetHashCompoFile();
    FicheroIni.sync();
    //Aqui le calculamos un hash

}

/** Marca el fichero ini de composición*/
void In2CompoFile::SetHashCompoFile(){
    //Borra el hash
    QSettings FicheroIni(memoryMap->m_mapa.m_variables.NombreRutaIni,QSettings::IniFormat,0);
    FicheroIni.remove(TXT_HASH_KEY);
    In2Hash p;
    QByteArray compoHash=p.GenerateHashFromFile(memoryMap->m_mapa.m_variables.NombreRutaIni);
    FicheroIni.beginGroup(TXT_HASH_KEY);
    FicheroIni.setValue(TXT_HASH_VALUE,QString(compoHash));
    FicheroIni.endGroup();
    FicheroIni.sync();
}
/** Calcula el hash de un fichero de composicion*/
QString In2CompoFile::CalculateHashCompoFile(){
    QString tmpLoc=QString("%1\\tmp.ini").arg(QApplication::applicationDirPath());
    QFile::copy(memoryMap->m_mapa.m_variables.NombreRutaIni,tmpLoc);
    //Borra el hash
    QSettings FicheroIni(tmpLoc,QSettings::IniFormat,0);
    FicheroIni.remove(TXT_HASH_KEY);
    FicheroIni.sync();
    In2Hash p;
    QByteArray compoHash=p.GenerateHashFromFile(tmpLoc);
    QFile::remove(tmpLoc);
    return QString(compoHash);
}
/** Devuelve el hash de un fichero de composicion*/
QString In2CompoFile::GetHashCompoFile(){
    QSettings FicheroIni(memoryMap->m_mapa.m_variables.NombreRutaIni,QSettings::IniFormat,0);
    FicheroIni.beginGroup(TXT_HASH_KEY);
    QString hash=FicheroIni.value(TXT_HASH_VALUE,"").toString();
    return hash;
}


/** Abre el fichero de composición*/
void In2CompoFile::AbrirFicheroIni(){

    QString dummy_string;
    QString dummy_string2;
    //double dummy_double;
    int dummy_int;

    dummy_string=QString(memoryMap->m_mapa.m_variables.NombreRutaIni);
    QSettings FicheroIni(dummy_string,QSettings::IniFormat,0);

    FicheroIni.beginGroup(TXT_PROPIEDADES);
    memoryMap->m_mapa.m_variables.CreadoPor=FicheroIni.value(TXT_CREADOPOR,"").toString();
    memoryMap->m_mapa.m_variables.CreadoCuando=FicheroIni.value(TXT_CREADOCUANDO,"").toString();
    memoryMap->m_mapa.m_variables.ModificadoPor=FicheroIni.value(TXT_MODIFICADOPOR,"").toString();
    memoryMap->m_mapa.m_variables.ModificadoCuando=FicheroIni.value(TXT_MODIFICADOCUANDO,"").toString();
    //Aqui comprobamos si los campos están vacíos...entonces muestro los metadatos.
    QFileInfo metaData(memoryMap->m_mapa.m_variables.NombreRutaIni);
    if (memoryMap->m_mapa.m_variables.CreadoPor.isEmpty()) memoryMap->m_mapa.m_variables.CreadoPor=metaData.owner();
    if (memoryMap->m_mapa.m_variables.CreadoCuando.isEmpty()) {
        QDateTime date=metaData.created();
        memoryMap->m_mapa.m_variables.CreadoCuando=date.toString("dd/MM/yy hh:mm:ss");
    }
    if (memoryMap->m_mapa.m_variables.ModificadoPor.isEmpty()) memoryMap->m_mapa.m_variables.ModificadoPor=metaData.owner();
    if (memoryMap->m_mapa.m_variables.ModificadoCuando.isEmpty()) {
        QDateTime date=metaData.lastModified();
        memoryMap->m_mapa.m_variables.ModificadoCuando=date.toString("dd/MM/yy hh:mm:ss");
    }

    FicheroIni.endGroup();


    FicheroIni.beginGroup(TXT_IMAGEN_FONDO);

    memoryMap->m_mapa.m_variables.bPlantEnabled=FicheroIni.value(TXT_PLANTENABLED,0).toBool();
    if (memoryMap->m_mapa.m_variables.bPlantEnabled){
        memoryMap->m_mapa.m_variables.fPlantAncho=FicheroIni.value(TXT_PLANTWIDTH,DEFAULT_WEB_WIDTH).toDouble();
        memoryMap->m_mapa.m_variables.fPlantAlto=FicheroIni.value(TXT_PLANTHEIGHT,DEFAULT_WEB_HEIGHT).toDouble();
        memoryMap->m_mapa.m_variables.fPlantOffsetX=FicheroIni.value(TXT_PLANTOFFSETX,0).toDouble();
        memoryMap->m_mapa.m_variables.fPlantOffsetY=FicheroIni.value(TXT_PLANTOFFSETY,0).toDouble();
        memoryMap->m_mapa.m_variables.fPlantDistCols=FicheroIni.value(TXT_PLANTDISTCOLS,0).toDouble();
        memoryMap->m_mapa.m_variables.iPlantCols=FicheroIni.value(TXT_PLANTCOLS,1).toInt();
        memoryMap->m_mapa.m_variables.strPlantImages.clear();
        for (int x=0;x<memoryMap->m_mapa.m_variables.iPlantCols;x++){
            QString file=QString("%1%2").arg(TXT_PLANTFILE).arg(x);
            memoryMap->m_mapa.m_variables.strPlantImages.append(FicheroIni.value(file,"").toString());
        }
    }


    dummy_string=FicheroIni.value(TXT_NOMBRE_PDF,"").toString();
    memoryMap->m_mapa.m_variables.NombrePdf=dummy_string;
    dummy_string=FicheroIni.value(TXT_RUTA_PDF,"").toString();
    dummy_string.replace("/","\\");
    memoryMap->m_mapa.m_variables.RutaPdf=dummy_string;
    memoryMap->m_mapa.m_variables.NombreRutaPdf=QString("%1\\%2").arg(memoryMap->m_mapa.m_variables.RutaPdf).arg(memoryMap->m_mapa.m_variables.NombrePdf);
    memoryMap->m_mapa.m_variables.XinicialPlantilla=FicheroIni.value(TXT_X_INICIAL_IMAGEN_FONDO,0).toDouble();
    memoryMap->m_mapa.m_variables.YinicialPlantilla=FicheroIni.value(TXT_Y_INICIAL_IMAGEN_FONDO,0).toDouble();
    memoryMap->m_mapa.m_variables.AnchoPlantilla=FicheroIni.value(TXT_ANCHO_IMAGEN_FONDO,DEFAULT_WEB_WIDTH).toDouble();
    memoryMap->m_mapa.m_variables.AltoPlantilla=FicheroIni.value(TXT_ALTO_IMAGEN_FONDO,DEFAULT_WEB_HEIGHT).toDouble();
    memoryMap->m_mapa.m_variables.AlgdobleRes=FicheroIni.value(TXT_DOBLE_RES,false).toBool();
    memoryMap->m_mapa.m_variables.iBitsDithering=FicheroIni.value(TXT_BITS_DITH,1).toInt();
    memoryMap->m_mapa.m_variables.bDilateAlgorithm=FicheroIni.value(TXT_DILATE_ALG,false).toBool();
    memoryMap->m_mapa.m_variables.iSabreAlgorithm=FicheroIni.value(TXT_SABRE_ALG,0).toInt();
    memoryMap->m_mapa.m_variables.FilasPlantilla=memoryMap->m_mapa.m_variables.AltoPlantilla;
    memoryMap->m_mapa.m_variables.ColumnasPlantilla=memoryMap->m_mapa.m_variables.AnchoPlantilla;

    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_DB);
    dummy_string=FicheroIni.value(TXT_DB_NOMBRE,"").toString();
    memoryMap->m_mapa.m_database.NombreDb=QString(dummy_string);
    dummy_string=FicheroIni.value(TXT_DB_RUTA,"").toString();
    dummy_string.replace("/","\\");
    memoryMap->m_mapa.m_database.RutaDb=QString(dummy_string);
    dummy_string=FicheroIni.value(TXT_DB_TABLA_DATOS,"").toString();
    memoryMap->m_mapa.m_database.NombreTablaDatosImprimir=QString(dummy_string);

    if ((!memoryMap->m_mapa.m_database.NombreDb.isEmpty())&&(!memoryMap->m_mapa.m_database.RutaDb.isEmpty())){
        memoryMap->m_mapa.m_database.NombreRutaDb=QString("%1\\%2").arg(memoryMap->m_mapa.m_database.RutaDb).arg(memoryMap->m_mapa.m_database.NombreDb);

    }
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_MAQUINA);

    memoryMap->m_mapa.m_impresion.iVelocidad=FicheroIni.value(TXT_VELOCIDAD,DEFAULT_SPEED).toInt();
    memoryMap->m_mapa.m_impresion.qstrPal1=FicheroIni.value(TXT_PAL1,DEFAULT_XPALETTE).toString();
    memoryMap->m_mapa.m_impresion.qstrPal2=FicheroIni.value(TXT_PAL2,DEFAULT_XPALETTE).toString();
    memoryMap->m_mapa.m_impresion.qstrPal3=FicheroIni.value(TXT_PAL3,DEFAULT_XPALETTE).toString();
    memoryMap->m_mapa.m_impresion.qstrPal4=FicheroIni.value(TXT_PAL4,DEFAULT_XPALETTE).toString();

    FicheroIni.endGroup();


    FicheroIni.beginGroup(TXT_COLOR);
    dummy_string=FicheroIni.value(TXT_TIPO_COLOR,"").toString();
    memoryMap->m_mapa.m_variables.TipoColor=dummy_string;
    memoryMap->m_mapa.m_variables.NumColores=FicheroIni.value(TXT_VALOR_COLOR,255).toInt();
    memoryMap->m_mapa.m_variables.AlgSelColor=FicheroIni.value(TXT_ALG_SELECCION_COLOR,SELECCION_MANUAL).toInt();
    memoryMap->m_mapa.m_variables.iGrayTh=FicheroIni.value(TXT_GRAY_THRESHOLD,10).toInt();
    FicheroIni.endGroup();

    if (memoryMap->m_mapa.m_variables.NumColores==255){
        //No está el campo color en el fichero ini y será monocromo
        dummy_string=QString("%1").arg(SC_PLENOS);
        memoryMap->m_mapa.m_variables.TipoColor=dummy_string;
        memoryMap->m_mapa.m_variables.NumColores=1;
    }

    //qDebug() << "Espacio de color: " << memoryMap->m_mapa.m_variables.TipoColor;
    //qDebug() << "Numero de colores: " << memoryMap->m_mapa.m_variables.NumColores;

    //Para saber en qué version de campos estamos
    //VERSION_CAMPOS_1 = sólo una matriz por campo
    //VERSION_CAMPOS_2 = varias matrices por campo
    FicheroIni.beginGroup(txt_CAMPO.at(0));
    dummy_string=QString("M1_%1").arg(TXT_FILAS_MATRIZ_CAMPO);
    dummy_int=FicheroIni.value(dummy_string,-1).toInt();
    if (dummy_int==-1)  memoryMap->m_mapa.m_variables.version_campos=VERSION_CAMPOS_1;
    else memoryMap->m_mapa.m_variables.version_campos=VERSION_CAMPOS_2;
    qDebug()<< "Version campos actual: " << memoryMap->m_mapa.m_variables.version_campos;
    FicheroIni.endGroup();


    //Marca superior
    FicheroIni.beginGroup(TXT_MARCASUP);
    memoryMap->m_mapa.m_Marca.anguloSup=FicheroIni.value(TXT_ANGULO_COMPOSICION,0).toInt();
    memoryMap->m_mapa.m_Marca.MarcaSupFrecuencia=FicheroIni.value(TXT_FRECUENCIA_MACULA,0).toDouble();
    memoryMap->m_mapa.m_Marca.MarcaSupOnOff=FicheroIni.value(TXT_ON,false).toBool();
    memoryMap->m_mapa.m_Marca.MarcaSupRepeticiones=FicheroIni.value(TXT_REPETICIONES_MACULA,0).toInt();
    memoryMap->m_mapa.m_Marca.MarcaSupTipo=FicheroIni.value(TXT_TIPO_CAMPO,0).toInt();
    memoryMap->m_mapa.m_Marca.MarcaSupValor=FicheroIni.value(TXT_VALOR_CAMPO,"").toString();
    memoryMap->m_mapa.m_Marca.MarcaSupX=FicheroIni.value(TXT_X_INICIAL_IMAGEN_FONDO,0).toDouble();
    memoryMap->m_mapa.m_Marca.MarcaSupY=FicheroIni.value(TXT_Y_INICIAL_IMAGEN_FONDO,0).toDouble();
    struct m_BarcodeParameters bc=memoryMap->m_mapa.m_Marca.barcodeSup;
    bc.width_mm=FicheroIni.value(TXT_WIDTH_BARCODE_MM,0).toDouble();
    bc.height_mm=FicheroIni.value(TXT_HEIGHT_BARCODE_MM,0).toDouble();
    bc.width=FicheroIni.value(TXT_WIDTH_BARCODE,0).toDouble();
    bc.height=FicheroIni.value(TXT_HEIGHT_BARCODE,0).toDouble();

    bc.codificacion=FicheroIni.value(TXT_CODIFICACION_BARCODE,0).toInt();
    bc.textAlignement=FicheroIni.value(TXT_TEXT_ALIGNMENT,0).toInt();
    bc.option2=FicheroIni.value(TXT_OPTION2_BARCODE,0).toInt();
    bc.inputMode=FicheroIni.value(TXT_INPUTMODE_BARCODE,0).toInt();
    bc.indiceFgColor=FicheroIni.value(TXT_FGCOLOR_BARCODE,0).toInt();
    bc.indiceBgColor=FicheroIni.value(TXT_BGCOLOR_BARCODE,0).toInt();
    bc.borderType=FicheroIni.value(TXT_BORDERTYPE_BARCODE,0).toInt();
    bc.borderWidth=FicheroIni.value(TXT_BORDERWIDTH_BARCODE,0).toInt();
    bc.whitespace=FicheroIni.value(TXT_WHITESPACE_BARCODE,0).toInt();
    bc.escala=FicheroIni.value(TXT_SCALE_BARCODE,0).toInt();
    bc.ar=(Zint::QZint::AspectRatioMode)FicheroIni.value(TXT_ASPECTRATIO_BARCODE,0).toInt();
    dummy_string=FicheroIni.value(QString("%1").arg(TXT_ESCONDERTEXTO_BARCODE),"Si").toString();
    if (!dummy_string.compare("Si",Qt::CaseSensitive)) bc.hideText=true;
    else bc.hideText=false;
    bc.fontSizeLarge=FicheroIni.value(TXT_FONTLARGE_BARCODE,9).toInt();
    bc.fontSizeSmall=FicheroIni.value(TXT_FONTSMALL_BARCODE,9).toInt();
    bc.fontType=FicheroIni.value(TXT_FONTTYPE_BARCODE,"").toString();
    bc.nivelSeguridad=FicheroIni.value(TXT_SEGURIDAD_BARCODE,0).toInt();
    FicheroIni.endGroup();
    memoryMap->m_mapa.m_Marca.barcodeSup=bc;




    //Marca inferior
    bc=memoryMap->m_mapa.m_Marca.barcodeInf;
    FicheroIni.beginGroup(TXT_MARCAINF);
    memoryMap->m_mapa.m_Marca.anguloInf=FicheroIni.value(TXT_ANGULO_COMPOSICION,0).toInt();
    memoryMap->m_mapa.m_Marca.MarcaInfFrecuencia=FicheroIni.value(TXT_FRECUENCIA_MACULA,0).toDouble();
    memoryMap->m_mapa.m_Marca.MarcaInfOnOff=FicheroIni.value(TXT_ON,false).toBool();
    memoryMap->m_mapa.m_Marca.MarcaInfRepeticiones=FicheroIni.value(TXT_REPETICIONES_MACULA,0).toInt();
    memoryMap->m_mapa.m_Marca.MarcaInfTipo=FicheroIni.value(TXT_TIPO_CAMPO,0).toInt();
    memoryMap->m_mapa.m_Marca.MarcaInfValor=FicheroIni.value(TXT_VALOR_CAMPO,"").toString();
    memoryMap->m_mapa.m_Marca.MarcaInfX=FicheroIni.value(TXT_X_INICIAL_IMAGEN_FONDO,0).toDouble();
    memoryMap->m_mapa.m_Marca.MarcaInfY=FicheroIni.value(TXT_Y_INICIAL_IMAGEN_FONDO,0).toDouble();
    bc.width_mm=FicheroIni.value(TXT_WIDTH_BARCODE_MM,0).toDouble();
    bc.height_mm=FicheroIni.value(TXT_HEIGHT_BARCODE_MM,0).toDouble();
    bc.width=FicheroIni.value(TXT_WIDTH_BARCODE,0).toDouble();
    bc.height=FicheroIni.value(TXT_HEIGHT_BARCODE,0).toDouble();

    bc.codificacion=FicheroIni.value(TXT_CODIFICACION_BARCODE,0).toInt();
    bc.textAlignement=FicheroIni.value(TXT_TEXT_ALIGNMENT,0).toInt();
    bc.option2=FicheroIni.value(TXT_OPTION2_BARCODE,0).toInt();
    bc.inputMode=FicheroIni.value(TXT_INPUTMODE_BARCODE,0).toInt();
    bc.indiceFgColor=FicheroIni.value(TXT_FGCOLOR_BARCODE,0).toInt();
    bc.indiceBgColor=FicheroIni.value(TXT_BGCOLOR_BARCODE,0).toInt();
    bc.borderType=FicheroIni.value(TXT_BORDERTYPE_BARCODE,0).toInt();
    bc.borderWidth=FicheroIni.value(TXT_BORDERWIDTH_BARCODE,0).toInt();
    bc.whitespace=FicheroIni.value(TXT_WHITESPACE_BARCODE,0).toInt();
    bc.escala=FicheroIni.value(TXT_SCALE_BARCODE,0).toInt();
    bc.ar=(Zint::QZint::AspectRatioMode)FicheroIni.value(TXT_ASPECTRATIO_BARCODE,0).toInt();
    dummy_string=FicheroIni.value(QString("%1").arg(TXT_ESCONDERTEXTO_BARCODE),"Si").toString();
    if (!dummy_string.compare("Si",Qt::CaseSensitive)) bc.hideText=true;
    else bc.hideText=false;
    bc.fontSizeLarge=FicheroIni.value(TXT_FONTLARGE_BARCODE,0).toInt();
    bc.fontSizeSmall=FicheroIni.value(TXT_FONTSMALL_BARCODE,0).toInt();
    bc.fontType=FicheroIni.value(TXT_FONTTYPE_BARCODE,"").toString();
    bc.nivelSeguridad=FicheroIni.value(TXT_SEGURIDAD_BARCODE,0).toInt();
    FicheroIni.endGroup();
    memoryMap->m_mapa.m_Marca.barcodeInf=bc;



    QString dummy_txt;

    for(int i=0;i<NUM_CAMPOS;i++){
        dummy_txt=txt_CAMPO.at(i);
        FicheroIni.beginGroup(txt_CAMPO.at(i));

        dummy_string=FicheroIni.value(TXT_NOMBRE_CAMPO,"Unknown").toString();
        //strncpy(memoryMap->m_mapa.m_campos[i].NombreCampo,dummy_string.toAscii().data(),CAMPOS_LEN);
        memoryMap->m_mapa.m_campos[i].NombreCampo=dummy_string;

        memoryMap->m_mapa.m_campos[i].TipoCampo=FicheroIni.value(TXT_TIPO_CAMPO,TXT_CAMPO_TIPO_TEXTO).toString();
        memoryMap->m_mapa.m_campos[i].m_dateTimeParameters.formato=FicheroIni.value(TXT_DATETIME_FORMATO,"").toString();

        if  (memoryMap->m_mapa.m_campos[i].TipoCampo==QString("%1").arg(TXT_CAMPO_TIPO_BARCODE)) {
             //Parametros para definir el código de barras
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.NumeroCampo=i;
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.width_mm=FicheroIni.value(QString("%1").arg(TXT_WIDTH_BARCODE),64).toDouble();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.height_mm=FicheroIni.value(QString("%1").arg(TXT_HEIGHT_BARCODE),64).toDouble();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.width=(int) (memoryMap->m_mapa.m_campos[i].m_barcodeParameters.width_mm*FACTOR_PIXELS_MM);
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.height=(int) (memoryMap->m_mapa.m_campos[i].m_barcodeParameters.height_mm*FACTOR_PIXELS_MM);
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.codificacion=FicheroIni.value(QString("%1").arg(TXT_CODIFICACION_BARCODE),20).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.textAlignement=FicheroIni.value(QString("%1").arg(TXT_TEXT_ALIGNMENT),Qt::AlignHCenter).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.option2=FicheroIni.value(QString("%1").arg(TXT_OPTION2_BARCODE),1).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.inputMode=FicheroIni.value(QString("%1").arg(TXT_INPUTMODE_BARCODE),0).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.indiceFgColor=FicheroIni.value(QString("%1").arg(TXT_FGCOLOR_BARCODE),1).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.indiceBgColor=FicheroIni.value(QString("%1").arg(TXT_BGCOLOR_BARCODE),1).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.borderType=FicheroIni.value(QString("%1").arg(TXT_BORDERTYPE_BARCODE),0).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.borderWidth=FicheroIni.value(QString("%1").arg(TXT_BORDERWIDTH_BARCODE),0).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.whitespace=FicheroIni.value(QString("%1").arg(TXT_WHITESPACE_BARCODE),0).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.escala=1;
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.ar=(Zint::QZint::AspectRatioMode)FicheroIni.value(QString("%1").arg(TXT_ASPECTRATIO_BARCODE),0).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.fontSizeLarge=FicheroIni.value(QString("%1").arg(TXT_FONTLARGE_BARCODE),10).toInt();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.fontSizeSmall=FicheroIni.value(QString("%1").arg(TXT_FONTSMALL_BARCODE),9).toInt();
            dummy_string=FicheroIni.value(QString("%1").arg(TXT_ESCONDERTEXTO_BARCODE),"Si").toString();
            if (!dummy_string.compare("Si",Qt::CaseSensitive)) memoryMap->m_mapa.m_campos[i].m_barcodeParameters.hideText=true;
            else memoryMap->m_mapa.m_campos[i].m_barcodeParameters.hideText=false;
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.fontType=FicheroIni.value(QString("%1").arg(TXT_FONTTYPE_BARCODE),"Arial").toString();
            memoryMap->m_mapa.m_campos[i].m_barcodeParameters.nivelSeguridad=FicheroIni.value(QString("%1").arg(TXT_SEGURIDAD_BARCODE),0).toInt();
        }

        dummy_string=FicheroIni.value(TXT_FUENTE_CAMPO,"Arial").toString();
        //strncpy(memoryMap->m_mapa.m_campos[i].TipoLetraCampo,dummy_string.toAscii().data(),CAMPOS_LEN);
        memoryMap->m_mapa.m_campos[i].TipoLetraCampo=dummy_string;

        dummy_string=FicheroIni.value(TXT_ESTADO_CAMPO,"Off").toString();
        if (dummy_string==TXT_ON) memoryMap->m_mapa.m_campos[i].OnOffCampo=1;
        else memoryMap->m_mapa.m_campos[i].OnOffCampo=0;

        memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo=FicheroIni.value(TXT_TAMANO_CAMPO,12).toInt();
        memoryMap->m_mapa.m_campos[i].letterSpace=FicheroIni.value(TXT_LETTER_SPACE,0).toInt();
        memoryMap->m_mapa.m_campos[i].ValorActualCampo=FicheroIni.value(TXT_VALOR_CAMPO,QString("Texto campo %1").arg(i)).toString();
        memoryMap->m_mapa.m_campos[i].NumCaracteresCampo=FicheroIni.value(TXT_NUM_CARACTERES_CAMPO,0).toInt();

        dummy_string=FicheroIni.value(TXT_ESTILO_CAMPO,"Normal").toString();
        //strncpy(memoryMap->m_mapa.m_campos[i].EstiloLetraCampo,dummy_string.toAscii().data(),CAMPOS_LEN);
        memoryMap->m_mapa.m_campos[i].EstiloLetraCampo=dummy_string;

        if (memoryMap->m_mapa.m_variables.NumColores>1){
            memoryMap->m_mapa.m_campos[i].color=FicheroIni.value(TXT_COLOR_CAMPO,1).toInt();
        }

        if (memoryMap->m_mapa.m_variables.version_campos==VERSION_CAMPOS_1){
            dummy_string=FicheroIni.value(TXT_ESTADO_CAMPO,"Off").toString();
            if (dummy_string==TXT_ON) memoryMap->m_mapa.m_campos[i].m_matrices[0].OnOffMatriz=1;
            else memoryMap->m_mapa.m_campos[i].m_matrices[0].OnOffMatriz=0;

            memoryMap->m_mapa.m_campos[i].m_matrices[0].PosicionXCampo=FicheroIni.value(TXT_COORDENADA_X_CAMPO,0).toDouble();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].PosicionYCampo=FicheroIni.value(TXT_COORDENADA_Y_CAMPO,0).toDouble();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].OrientacionCampo=FicheroIni.value(TXT_ORIENTACION_CAMPO,0).toInt();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].FilasMatrizCampo=FicheroIni.value(TXT_FILAS_MATRIZ_CAMPO,1).toInt();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].ColumnasMatrizCampo=FicheroIni.value(TXT_COLUMNAS_MATRIZ_CAMPO,1).toInt();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].FreqFilasMatrizCampo=FicheroIni.value(TXT_FREQ_FILAS_MATRIZ_CAMPO,0).toDouble();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].FreqColumnasMatrizCampo=FicheroIni.value(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO,0).toDouble();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].incColumnas=FicheroIni.value(TXT_INC_COLUMNAS_MATRIZ_CAMPO,0).toDouble();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].incFilas=FicheroIni.value(TXT_INC_FILAS_MATRIZ_CAMPO,0).toDouble();

            dummy_string2=FicheroIni.value(TXT_EXCEPCIONES_MATRIZ_CAMPO,"").toString();
            memoryMap->m_mapa.m_campos[i].m_matrices[0].ExcepcionesMatrizCampo=dummy_string2;
            for (int j=1;j<NUM_MATRIZ_POR_CAMPO;j++){
                memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=0;
                memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo=0;
                memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo=(i*NUM_MATRIZ_POR_CAMPO+j)*DEFAULT_FONT_SIZE+2;
                memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo=0;
                memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo=1;
                memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo=1;
                memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo=0;
                memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo=0;
                dummy_string2=QString("");
                memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo=dummy_string2;
            }
        }

        if (memoryMap->m_mapa.m_variables.version_campos==VERSION_CAMPOS_2){
            for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ESTADO_CAMPO);
                dummy_string2=FicheroIni.value(dummy_string,TXT_OFF).toString();
                if (dummy_string2==TXT_ON) memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=1;
                else memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=0;

                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_X_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo=FicheroIni.value(dummy_string,0).toDouble();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_Y_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo=FicheroIni.value(dummy_string,(i*NUM_MATRIZ_POR_CAMPO+j)*DEFAULT_FONT_SIZE+2).toDouble();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ORIENTACION_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo=FicheroIni.value(dummy_string,0).toInt();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FILAS_MATRIZ_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo=FicheroIni.value(dummy_string,1).toInt();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COLUMNAS_MATRIZ_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo=FicheroIni.value(dummy_string,1).toInt();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_FILAS_MATRIZ_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo=FicheroIni.value(dummy_string,0).toDouble();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo=FicheroIni.value(dummy_string,0).toDouble();

                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_INC_COLUMNAS_MATRIZ_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].incColumnas=FicheroIni.value(dummy_string,0).toDouble();

                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_INC_FILAS_MATRIZ_CAMPO);
                memoryMap->m_mapa.m_campos[i].m_matrices[j].incFilas=FicheroIni.value(dummy_string,0).toDouble();


                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_EXCEPCIONES_MATRIZ_CAMPO);
                dummy_string2=FicheroIni.value(dummy_string,"").toString();
                memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo=dummy_string2;
            }
        }

        FicheroIni.endGroup();
    }


    memoryMap->ImagenVariable=0x00;
    //Meto este código para los campos variables
    for(int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        //dummy_txt=TXT_CAMPO_VARIABLE.at(i);
        FicheroIni.beginGroup(txt_CAMPO_VARIABLE.at(i));

        memoryMap->m_mapa.m_camposvariables[i].NombreCampo=FicheroIni.value(TXT_NOMBRE_CAMPO,"Unknown").toString();
        memoryMap->m_mapa.m_camposvariables[i].TipoCampo=FicheroIni.value(TXT_TIPO_CAMPO,TXT_CAMPO_TIPO_TEXTO).toString();
        memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo=FicheroIni.value(TXT_ORIGEN_VALOR_CAMPO,TXT_ORIGEN_CONTADOR).toString();

        if (!memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo.compare(TXT_ORIGEN_DB,Qt::CaseInsensitive)){
            memoryMap->m_mapa.m_camposvariables[i].indiceOrigenCampo=ID_ORIGEN_DB;
        }
        else if (!memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo.compare(TXT_ORIGEN_CONTADOR,Qt::CaseInsensitive)){
            memoryMap->m_mapa.m_camposvariables[i].indiceOrigenCampo=ID_ORIGEN_CONTADOR;
        }
        else if (!memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo.compare(TXT_ORIGEN_FECHA,Qt::CaseInsensitive)){
            memoryMap->m_mapa.m_camposvariables[i].indiceOrigenCampo=ID_ORIGEN_FECHA;
        }
        else if (!memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo.compare(TXT_ORIGEN_RANDOM,Qt::CaseInsensitive)){
            memoryMap->m_mapa.m_camposvariables[i].indiceOrigenCampo=ID_ORIGEN_RANDOM;
        }

        dummy_string=FicheroIni.value(TXT_ESTADO_CAMPO,"Off").toString();
        if (dummy_string==TXT_ON) {
            memoryMap->m_mapa.m_camposvariables[i].OnOffCampo=1;
            memoryMap->ImagenVariable=0x01;
        }
        else memoryMap->m_mapa.m_camposvariables[i].OnOffCampo=0;

        //Caso de que el campo haya que meterlo como texto
        if (memoryMap->m_mapa.m_camposvariables[i].TipoCampo==QString("%1").arg(TXT_CAMPO_TIPO_TEXTO)){
            memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo=FicheroIni.value(TXT_FUENTE_CAMPO,"Arial").toString();
            memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo=FicheroIni.value(TXT_TAMANO_CAMPO,12).toInt();
            memoryMap->m_mapa.m_camposvariables[i].letterSpace=FicheroIni.value(TXT_LETTER_SPACE,0).toInt();
            memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo=FicheroIni.value(TXT_ESTILO_CAMPO,"Normal").toString();
            memoryMap->m_mapa.m_camposvariables[i].NumCaracteresCampo=FicheroIni.value(TXT_NUM_CARACTERES_CAMPO,0).toInt();
        }
        else if  (memoryMap->m_mapa.m_camposvariables[i].TipoCampo==QString("%1").arg(TXT_CAMPO_TIPO_BARCODE)){
            memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo=FicheroIni.value(TXT_FUENTE_CAMPO,"Arial").toString();
            memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo=FicheroIni.value(TXT_TAMANO_CAMPO,12).toInt();
            memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo=FicheroIni.value(TXT_ESTILO_CAMPO,"Normal").toString();
            memoryMap->m_mapa.m_camposvariables[i].NumCaracteresCampo=FicheroIni.value(TXT_NUM_CARACTERES_CAMPO,0).toInt();
            //Parametros para definir el código de barras
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.NumeroCampo=i;
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.width_mm=FicheroIni.value(QString("%1").arg(TXT_WIDTH_BARCODE),64).toDouble();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.height_mm=FicheroIni.value(QString("%1").arg(TXT_HEIGHT_BARCODE),64).toDouble();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.width=(int) (memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.width_mm*FACTOR_PIXELS_MM);
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.height=(int) (memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.height_mm*FACTOR_PIXELS_MM);
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.codificacion=FicheroIni.value(QString("%1").arg(TXT_CODIFICACION_BARCODE),20).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.textAlignement=FicheroIni.value(QString("%1").arg(TXT_TEXT_ALIGNMENT),Qt::AlignHCenter).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.option2=FicheroIni.value(QString("%1").arg(TXT_OPTION2_BARCODE),1).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.inputMode=FicheroIni.value(QString("%1").arg(TXT_INPUTMODE_BARCODE),0).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.indiceFgColor=FicheroIni.value(QString("%1").arg(TXT_FGCOLOR_BARCODE),64).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.indiceBgColor=FicheroIni.value(QString("%1").arg(TXT_BGCOLOR_BARCODE),64).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.borderType=FicheroIni.value(QString("%1").arg(TXT_BORDERTYPE_BARCODE),0).toInt();;
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.borderWidth=FicheroIni.value(QString("%1").arg(TXT_BORDERWIDTH_BARCODE),0).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.whitespace=FicheroIni.value(QString("%1").arg(TXT_WHITESPACE_BARCODE),0).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.escala=1;
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.ar=(Zint::QZint::AspectRatioMode)FicheroIni.value(QString("%1").arg(TXT_ASPECTRATIO_BARCODE),0).toInt();;
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.fontSizeLarge=FicheroIni.value(QString("%1").arg(TXT_FONTLARGE_BARCODE),10).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.fontSizeSmall=FicheroIni.value(QString("%1").arg(TXT_FONTSMALL_BARCODE),9).toInt();
            dummy_string=FicheroIni.value(QString("%1").arg(TXT_ESCONDERTEXTO_BARCODE),"Si").toString();
            if (!dummy_string.compare("Si",Qt::CaseSensitive)) memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.hideText=true;
            else memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.hideText=false;
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.fontType=FicheroIni.value(QString("%1").arg(TXT_FONTTYPE_BARCODE),"Arial").toString();
            memoryMap->m_mapa.m_camposvariables[i].m_barcodeParameters.nivelSeguridad=FicheroIni.value(QString("%1").arg(TXT_SEGURIDAD_BARCODE),0).toInt();
        }

        //Caso de que el campo sea de tipo contador
        if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.numeroCifras=FicheroIni.value(TXT_CONTADOR_NUMERO_CIFRAS,10).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.limiteInferior=FicheroIni.value(TXT_CONTADOR_LIMITE_INFERIOR,0).toLongLong();
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.limiteSuperior=FicheroIni.value(TXT_CONTADOR_LIMITE_SUPERIOR,1000).toLongLong();
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial=FicheroIni.value(TXT_CONTADOR_VALOR_INICIAL,0).toLongLong();
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicial;
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.incremento=FicheroIni.value(TXT_CONTADOR_INCREMENTO,1).toLongLong();
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.configuracion=FicheroIni.value(TXT_CONTADOR_CONFIGURACION,TXT_CONTADOR_CONFIGURACION_FILAS).toString();
            if (!memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.configuracion.compare(TXT_CONTADOR_CONFIGURACION_FILAS,Qt::CaseInsensitive)){
                memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion=ID_CONTADOR_CONFIGURACION_FILAS;
            }
            else{
                memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion=ID_CONTADOR_CONFIGURACION_COLUMNAS;
            }
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset=FicheroIni.value(TXT_CONTADOR_OFFSET,0).toLongLong();
            //Formato
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.numeroCifras=FicheroIni.value(TXT_CONTADOR_NUMERO_CIFRAS,10).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.zeroPadding=FicheroIni.value(TXT_CONTADOR_ZPADDING,false).toBool();
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.bFormatEnabled=FicheroIni.value(TXT_CONTADOR_FORMATENABLED,false).toBool();
            QLocale locale;
            memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.m_format.FormatSeparador=(QLocale::Language)FicheroIni.value(TXT_CONTADOR_FORMATLANGUAJE,locale.language()).toInt();
        }
        //Caso de que el campo sea de tipo fecha/hora
        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_FECHA)){
            memoryMap->m_mapa.m_camposvariables[i].m_dateTimeParameters.formato=FicheroIni.value(TXT_DATETIME_FORMATO,"").toString();
        }
        //Caso de que el campo sea de tipo aleatorio
        else if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_RANDOM)){
            memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteInferior=FicheroIni.value(TXT_RANDOM_LIMITE_INFERIOR,0).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.limiteSuperior=FicheroIni.value(TXT_RANDOM_LIMITE_SUPERIOR,1000).toInt();
            memoryMap->m_mapa.m_camposvariables[i].m_randomNumberParameters.numeroCifras=FicheroIni.value(TXT_RANDOM_NUMERO_CIFRAS,4).toInt();
        }

        if (memoryMap->m_mapa.m_variables.NumColores>1){
            memoryMap->m_mapa.m_camposvariables[i].color=FicheroIni.value(TXT_COLOR_CAMPO,1).toInt();
        }

        for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ESTADO_CAMPO);
            dummy_string2=FicheroIni.value(dummy_string,"").toString();
            if (dummy_string2==TXT_ON) memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz=1;
            else memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz=0;

            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_X_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo=FicheroIni.value(dummy_string,(i*NUM_MATRIZ_POR_CAMPO+j)*DEFAULT_FONT_SIZE+2).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_Y_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo=FicheroIni.value(dummy_string,0).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ORIENTACION_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo=FicheroIni.value(dummy_string,0).toInt();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FILAS_MATRIZ_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo=FicheroIni.value(dummy_string,1).toInt();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COLUMNAS_MATRIZ_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo=FicheroIni.value(dummy_string,1).toInt();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_FILAS_MATRIZ_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo=FicheroIni.value(dummy_string,0).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo=FicheroIni.value(dummy_string,0).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_INC_COLUMNAS_MATRIZ_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incColumnas=FicheroIni.value(dummy_string,0).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_INC_FILAS_MATRIZ_CAMPO);
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].incFilas=FicheroIni.value(dummy_string,0).toDouble();


            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_EXCEPCIONES_MATRIZ_CAMPO);
            dummy_string2=FicheroIni.value(dummy_string,"").toString();
            memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo=dummy_string2;
        }

        FicheroIni.endGroup();
    }


    FicheroIni.beginGroup(TXT_ORIGEN);
    memoryMap->m_mapa.m_variables.PuntoInicialX=FicheroIni.value(TXT_COORDENADA_X_ORIGEN,0).toDouble();
    memoryMap->m_mapa.m_variables.PuntoInicialY=FicheroIni.value(TXT_COORDENADA_Y_ORIGEN,0).toDouble();
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_COMPOSICION);
    memoryMap->m_mapa.m_variables.Comentario=FicheroIni.value(TXT_COMENTARIO_COMPOSICION,"").toString();
    memoryMap->m_mapa.m_variables.WebWidth=FicheroIni.value(TXT_WEBWIDTH_COMPOSICION,DEFAULT_WEB_WIDTH).toDouble();
    memoryMap->m_mapa.m_variables.WebHeight=FicheroIni.value(TXT_WEBHEIGHT_COMPOSICION,DEFAULT_WEB_HEIGHT).toDouble();
    memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas=FicheroIni.value(TXT_FILAS_COMPOSICION,1).toInt();
    memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas=FicheroIni.value(TXT_COLUMNAS_COMPOSICION,1).toInt();
    memoryMap->m_mapa.m_variables.SeparacionY=FicheroIni.value(TXT_SEPARACION_FILAS_COMPOSICION,0).toDouble();
    memoryMap->m_mapa.m_variables.SeparacionX=FicheroIni.value(TXT_SEPARACION_COLUMNAS_COMPOSICION,0).toDouble();
    memoryMap->m_mapa.m_variables.AnguloPlantilla=FicheroIni.value(TXT_ANGULO_COMPOSICION,0).toFloat();
    memoryMap->m_mapa.m_variables.ImpresionesPorEnvio=memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    FicheroIni.endGroup();


    FicheroIni.beginGroup(TXT_MACULA_IZQDA);
    dummy_string=FicheroIni.value(TXT_ESTADO_MACULA,TXT_OFF).toString();
    if (dummy_string==TXT_ON) memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=1;
    else memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=0;
    memoryMap->m_mapa.m_variables.MaculaIzqdaX=FicheroIni.value(TXT_COORDENADA_X_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaIzqdaY=FicheroIni.value(TXT_COORDENADA_Y_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaIzqdaAncho=FicheroIni.value(TXT_ANCHO_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaIzqdaAlto=FicheroIni.value(TXT_ALTO_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones=FicheroIni.value(TXT_REPETICIONES_MACULA,0).toInt();
    memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia=FicheroIni.value(TXT_FRECUENCIA_MACULA,1).toDouble();
    if (memoryMap->m_mapa.m_variables.NumColores>1){
        memoryMap->m_mapa.m_variables.MaculaIzqdaColor=FicheroIni.value(TXT_COLOR_MACULA,1024).toInt();
    }
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_MACULA_DCHA);
    dummy_string=FicheroIni.value(TXT_ESTADO_MACULA,1024).toString();
    if (dummy_string==TXT_ON) memoryMap->m_mapa.m_variables.MaculaDchaOnOff=1;
    else memoryMap->m_mapa.m_variables.MaculaDchaOnOff=0;
    memoryMap->m_mapa.m_variables.MaculaDchaX=FicheroIni.value(TXT_COORDENADA_X_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaDchaY=FicheroIni.value(TXT_COORDENADA_Y_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaDchaAncho=FicheroIni.value(TXT_ANCHO_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaDchaAlto=FicheroIni.value(TXT_ALTO_MACULA,0).toDouble();
    memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones=FicheroIni.value(TXT_REPETICIONES_MACULA,0).toInt();
    memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia=FicheroIni.value(TXT_FRECUENCIA_MACULA,1).toDouble();
    if (memoryMap->m_mapa.m_variables.NumColores>1){
         memoryMap->m_mapa.m_variables.MaculaDchaColor=FicheroIni.value(TXT_COLOR_MACULA,1024).toInt();
    }
    FicheroIni.endGroup();

    if (memoryMap->m_mapa.m_variables.NumColores>1){
            for (int i=0;i<memoryMap->m_mapa.m_variables.NumColores;i++){
                dummy_string=QString("%1%2").arg(TXT_COLOR).arg(i+1);
                FicheroIni.beginGroup(dummy_string);
                memoryMap->m_mapa.m_colores[i].x_pix=FicheroIni.value(TXT_COORDENADA_X_COLOR,0).toInt();
                memoryMap->m_mapa.m_colores[i].y_pix=FicheroIni.value(TXT_COORDENADA_Y_COLOR,0).toInt();
                memoryMap->m_mapa.m_colores[i].cadena=FicheroIni.value(TXT_CADENA_COLOR,"").toString();
                memoryMap->m_mapa.m_colores[i].enabled=FicheroIni.value(TXT_ENABLED,true).toBool();
                memoryMap->m_mapa.m_colores[i].alg=FicheroIni.value(TXT_ALG_DITH,"").toString();
                memoryMap->m_mapa.m_colores[i].bWhiteCorr=FicheroIni.value(TXT_ALG_WHITE,false).toBool();
                memoryMap->m_mapa.m_colores[i].bBlackCorr=FicheroIni.value(TXT_ALG_BLACK,false).toBool();
                memoryMap->m_mapa.m_colores[i].bMonoMode=FicheroIni.value(TXT_ALG_MONO,false).toBool();
                memoryMap->m_mapa.m_colores[i].bCorrDinamica=FicheroIni.value(TXT_ALG_CORRDIN,false).toBool();
                memoryMap->m_mapa.m_colores[i].bMonoCorr=FicheroIni.value(TXT_ALG_CORRMONO,false).toBool();
                memoryMap->m_mapa.m_colores[i].index=FicheroIni.value(TXT_COLOR_INDEX,0).toInt();
                memoryMap->m_mapa.m_colores[i].m_diherCut=FicheroIni.value(TXT_COLOR_DITHERCUT,CUT_LINEAL).toInt();
                memoryMap->m_mapa.m_colores[i].m_ditherPal=FicheroIni.value(TXT_COLOR_DITHERPAL,"").toString();
                memoryMap->m_mapa.m_colores[i].m_customPal=FicheroIni.value(TXT_COLOR_CUSTOMPAL,false).toBool();
                FicheroIni.endGroup();
            }
    }
    else{
        int i=1; //Chapucilla para hacer compatible con versiones que no guarden color
        dummy_string=QString("%1%2").arg(TXT_COLOR).arg(i);
        FicheroIni.beginGroup(dummy_string);
        memoryMap->m_mapa.m_colores[0].x_pix=FicheroIni.value(TXT_COORDENADA_X_COLOR,0).toInt();
        memoryMap->m_mapa.m_colores[0].y_pix=FicheroIni.value(TXT_COORDENADA_Y_COLOR,0).toInt();
        memoryMap->m_mapa.m_colores[0].cadena=FicheroIni.value(TXT_CADENA_COLOR,"").toString();
        memoryMap->m_mapa.m_colores[0].enabled=FicheroIni.value(TXT_ENABLED,true).toBool();
        memoryMap->m_mapa.m_colores[0].alg=FicheroIni.value(TXT_ALG_DITH,"").toString();
        memoryMap->m_mapa.m_colores[0].bWhiteCorr=FicheroIni.value(TXT_ALG_WHITE,false).toBool();
        memoryMap->m_mapa.m_colores[0].bBlackCorr=FicheroIni.value(TXT_ALG_BLACK,false).toBool();
        memoryMap->m_mapa.m_colores[0].bMonoMode=FicheroIni.value(TXT_ALG_MONO,false).toBool();
        memoryMap->m_mapa.m_colores[0].bCorrDinamica=FicheroIni.value(TXT_ALG_CORRDIN,false).toBool();
        memoryMap->m_mapa.m_colores[0].bMonoCorr=FicheroIni.value(TXT_ALG_CORRMONO,false).toBool();
        memoryMap->m_mapa.m_colores[0].index=FicheroIni.value(TXT_COLOR_INDEX,0).toInt();
        memoryMap->m_mapa.m_colores[0].m_diherCut=FicheroIni.value(TXT_COLOR_DITHERCUT,CUT_LINEAL).toInt();
        memoryMap->m_mapa.m_colores[0].m_ditherPal=FicheroIni.value(TXT_COLOR_DITHERPAL,"").toString();
        memoryMap->m_mapa.m_colores[0].m_customPal=FicheroIni.value(TXT_COLOR_CUSTOMPAL,false).toBool();

        FicheroIni.endGroup();

        memoryMap->m_mapa.m_colores[0].valor=QRgb(Qt::black);
        for(int i=0;i<NUM_CAMPOS;i++)  memoryMap->m_mapa.m_campos[i].color=1;
        for(int i=0;i<NUM_CAMPOS_VARIABLES;i++)  memoryMap->m_mapa.m_camposvariables[i].color=1;
        memoryMap->m_mapa.m_variables.MaculaIzqdaColor=1;
        memoryMap->m_mapa.m_variables.MaculaDchaColor=1;
    }

    FicheroIni.beginGroup(TXT_MASCARA_COLOR);
    memoryMap->m_mapa.m_ColorMascara.valor=QColor(Qt::white).rgb(); //Por defecto sin mascara
    memoryMap->m_mapa.m_ColorMascara.x_pix=FicheroIni.value(TXT_COORDENADA_X_COLOR,DEFAULT_NOMASK_PIXEL).toInt();
    memoryMap->m_mapa.m_ColorMascara.y_pix=FicheroIni.value(TXT_COORDENADA_Y_COLOR,DEFAULT_NOMASK_PIXEL).toInt();
    FicheroIni.endGroup();
}
/** Indica si el campo existe y esta activo*/
bool In2CompoFile::isFixedDataActive(QString val){
    bool rtn=false;
    for (int x=0;x<NUM_CAMPOS;x++){
        if (memoryMap->m_mapa.m_campos[x].OnOffCampo){
            if (!memoryMap->m_mapa.m_campos[x].NombreCampo.compare(val)){
                rtn=true;
                break;
            }
        }
    }
    return rtn;
}
