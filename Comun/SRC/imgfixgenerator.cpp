#include "imgfixgenerator.h"
#include "globals.h"
#include "MemoryMap.h"
#include <QtGlobal>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QImage>
#include <QThread>
#include <QSettings>
#include <QDebug>
#include <QPainter>

//Lista de pixmaps a generar para el caso de colores plenos
static QList<QPixmap> pixMapArray[NUM_COLORES];



ImgFixGenerator::ImgFixGenerator(QObject *parent,QString CurrentNombreRutaIni,int iCabezales) :
    QObject(parent)
{
    m_memoryMap=MemoryMap::request(0);
    NombreRutaIniActual=CurrentNombreRutaIni;
    m_NumCabezales=iCabezales;
}




void ImgFixGenerator::AbrirFicheroComposicionFija(){

    /*
    QString dummy_string;
    QString dummy_string1,dummy_string2;
    //double dummy_double;
    int dummy_int;

    QSettings FicheroIni(NombreRutaIniActual,QSettings::IniFormat,0);

    FicheroIni.beginGroup(TXT_IMAGEN_FONDO);
    dummy_string=FicheroIni.value(TXT_NOMBRE_PDF,1024).toString();
    strncpy(m_memoryMap->m_mapa.m_variables.NombrePdf,dummy_string.toAscii().data(),RUTA_LEN);
    dummy_string=FicheroIni.value(TXT_RUTA_PDF,1024).toString();
    strncpy(m_memoryMap->m_mapa.m_variables.RutaPdf,dummy_string.toAscii().data(),RUTA_LEN);
    m_memoryMap->m_mapa.m_variables.XinicialPlantilla=FicheroIni.value(TXT_X_INICIAL_IMAGEN_FONDO,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.YinicialPlantilla=FicheroIni.value(TXT_Y_INICIAL_IMAGEN_FONDO,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.AnchoPlantilla=FicheroIni.value(TXT_ANCHO_IMAGEN_FONDO,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.AltoPlantilla=FicheroIni.value(TXT_ALTO_IMAGEN_FONDO,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.FilasPlantilla=m_memoryMap->m_mapa.m_variables.AltoPlantilla;
    m_memoryMap->m_mapa.m_variables.ColumnasPlantilla=m_memoryMap->m_mapa.m_variables.AnchoPlantilla;
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_COLOR);
    dummy_string=FicheroIni.value(TXT_TIPO_COLOR,"").toString();
    strncpy(m_memoryMap->m_mapa.m_variables.TipoColor,dummy_string.toAscii().data(),RUTA_LEN);
    m_memoryMap->m_mapa.m_variables.NumColores=FicheroIni.value(TXT_VALOR_COLOR,255).toInt();
    FicheroIni.endGroup();

    if (m_memoryMap->m_mapa.m_variables.NumColores==255){
        //No está el campo color en el fichero ini y será monocromo
        dummy_string=QString("%1").arg(SC_MONOCROMO);
        strncpy(m_memoryMap->m_mapa.m_variables.TipoColor,dummy_string.toAscii().data(),RUTA_LEN);
        m_memoryMap->m_mapa.m_variables.NumColores=1;
    }

    qDebug() << "Espacio de color: " << m_memoryMap->m_mapa.m_variables.TipoColor;
    qDebug() << "Numero de colores: " << m_memoryMap->m_mapa.m_variables.NumColores;

    //Para saber en qué version de campos estamos
    //VERSION_CAMPOS_1 = sólo una matriz por campo
    //VERSION_CAMPOS_2 = varias matrices por campo
    dummy_string=QString("%1_1").arg(TXT_CAMPO);
    FicheroIni.beginGroup(dummy_string);
    dummy_string=QString("M1_%1").arg(TXT_FILAS_MATRIZ_CAMPO);
    dummy_int=FicheroIni.value(dummy_string,-1).toInt();
    if (dummy_int==-1)  m_memoryMap->m_mapa.m_variables.version_campos=VERSION_CAMPOS_1;
    else m_memoryMap->m_mapa.m_variables.version_campos=VERSION_CAMPOS_2;
    qDebug()<< "Version campos actual: " << m_memoryMap->m_mapa.m_variables.version_campos;
    FicheroIni.endGroup();

    QString dummy_txt;

    for(int i=0;i<NUM_CAMPOS;i++){


        dummy_txt=QString("%1_%2").arg(TXT_CAMPO).arg(i+1);
        qDebug() << dummy_txt;

        FicheroIni.beginGroup(dummy_txt);

        dummy_string=FicheroIni.value(TXT_NOMBRE_CAMPO,"Unknown").toString();
        strncpy(m_memoryMap->m_mapa.m_campos[i].NombreCampo,dummy_string.toAscii().data(),CAMPOS_LEN);

        dummy_string=FicheroIni.value(TXT_FUENTE_CAMPO,"Arial").toString();
        strncpy(m_memoryMap->m_mapa.m_campos[i].TipoLetraCampo,dummy_string.toAscii().data(),CAMPOS_LEN);

        dummy_string=FicheroIni.value(TXT_ESTADO_CAMPO,"Off").toString();
        if (dummy_string==TXT_ON) m_memoryMap->m_mapa.m_campos[i].OnOffCampo=1;
        else m_memoryMap->m_mapa.m_campos[i].OnOffCampo=0;

        m_memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo=FicheroIni.value(TXT_TAMANO_CAMPO,12).toInt();

        dummy_string=FicheroIni.value(TXT_ESTILO_CAMPO,"Normal").toString();
        strncpy(m_memoryMap->m_mapa.m_campos[i].EstiloLetraCampo,dummy_string.toAscii().data(),CAMPOS_LEN);

        if (m_memoryMap->m_mapa.m_variables.NumColores>1){
            m_memoryMap->m_mapa.m_campos[i].color=FicheroIni.value(TXT_COLOR_CAMPO,1).toInt();
        }

        if (m_memoryMap->m_mapa.m_variables.version_campos==VERSION_CAMPOS_1){
            dummy_string=FicheroIni.value(TXT_ESTADO_CAMPO,1024).toString();
            if (dummy_string==TXT_ON) m_memoryMap->m_mapa.m_campos[i].m_matrices[0].OnOffMatriz=1;
            else m_memoryMap->m_mapa.m_campos[i].m_matrices[0].OnOffMatriz=0;

            m_memoryMap->m_mapa.m_campos[i].m_matrices[0].PosicionXCampo=FicheroIni.value(TXT_COORDENADA_X_CAMPO,0).toDouble();
            m_memoryMap->m_mapa.m_campos[i].m_matrices[0].PosicionYCampo=FicheroIni.value(TXT_COORDENADA_Y_CAMPO,0).toDouble();
            //qDebug()<< "Y: " << m_memoryMap->m_mapa.m_campos[i].m_matrices[0].PosicionYCampo;
            m_memoryMap->m_mapa.m_campos[i].m_matrices[0].OrientacionCampo=FicheroIni.value(TXT_ORIENTACION_CAMPO,0).toInt();
            m_memoryMap->m_mapa.m_campos[i].m_matrices[0].FilasMatrizCampo=FicheroIni.value(TXT_FILAS_MATRIZ_CAMPO,0).toInt();
            m_memoryMap->m_mapa.m_campos[i].m_matrices[0].ColumnasMatrizCampo=FicheroIni.value(TXT_COLUMNAS_MATRIZ_CAMPO,0).toInt();
            m_memoryMap->m_mapa.m_campos[i].m_matrices[0].FreqFilasMatrizCampo=FicheroIni.value(TXT_FREQ_FILAS_MATRIZ_CAMPO,0).toDouble();
            m_memoryMap->m_mapa.m_campos[i].m_matrices[0].FreqColumnasMatrizCampo=FicheroIni.value(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO,0).toDouble();
            dummy_string2=FicheroIni.value(TXT_EXCEPCIONES_MATRIZ_CAMPO,"").toString();
            strncpy(m_memoryMap->m_mapa.m_campos[i].m_matrices[0].ExcepcionesMatrizCampo,dummy_string2.toAscii().data(),CAMPOS_LEN);
            //qDebug()<< "Excepciones: " << m_memoryMap->m_mapa.m_campos[i].m_matrices[0].ExcepcionesMatrizCampo;
            for (int j=1;j<NUM_MATRIZ_POR_CAMPO;j++){
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=0;
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo=0;
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo=0;
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo=0;
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo=0;
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo=0;
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo=0;
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo=0;
                dummy_string2=QString("");
                strncpy(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo,dummy_string2.toAscii().data(),CAMPOS_LEN);
            }
        }

        if (m_memoryMap->m_mapa.m_variables.version_campos==VERSION_CAMPOS_2){
            for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ESTADO_CAMPO);
                dummy_string2=FicheroIni.value(dummy_string,1024).toString();
                if (dummy_string2==TXT_ON) m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=1;
                else m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz=0;

                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_X_CAMPO);
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo=FicheroIni.value(dummy_string,1024).toDouble();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_Y_CAMPO);
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo=FicheroIni.value(dummy_string,1024).toDouble();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ORIENTACION_CAMPO);
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo=FicheroIni.value(dummy_string,1024).toInt();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FILAS_MATRIZ_CAMPO);
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo=FicheroIni.value(dummy_string,1024).toInt();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COLUMNAS_MATRIZ_CAMPO);
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo=FicheroIni.value(dummy_string,1024).toInt();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_FILAS_MATRIZ_CAMPO);
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo=FicheroIni.value(dummy_string,1024).toDouble();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO);
                m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo=FicheroIni.value(dummy_string,1024).toDouble();
                dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_EXCEPCIONES_MATRIZ_CAMPO);
                dummy_string2=FicheroIni.value(dummy_string,"").toString();
                strncpy(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo,dummy_string2.toAscii().data(),CAMPOS_LEN);
            }
        }

        FicheroIni.endGroup();
    }


    //Meto este código para los campos variables
    for(int i=0;i<NUM_CAMPOS_VARIABLES;i++){

        dummy_txt=QString("%1_%2").arg(TXT_CAMPO_VARIABLE).arg(i+1);

        FicheroIni.beginGroup(dummy_txt);
        m_memoryMap->m_mapa.m_camposvariables[i].NombreCampo=FicheroIni.value(TXT_NOMBRE_CAMPO,"Unknown").toString();
        m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo=FicheroIni.value(TXT_TIPO_CAMPO,TXT_CAMPO_TIPO_TEXTO).toString();
        m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo=FicheroIni.value(TXT_ORIGEN_VALOR_CAMPO,"Unknown").toString();

        dummy_string=FicheroIni.value(TXT_ESTADO_CAMPO,"Off").toString();
        if (dummy_string==TXT_ON) m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo=1;
        else m_memoryMap->m_mapa.m_camposvariables[i].OnOffCampo=0;

        //Caso de que el campo haya que meterlo como texto
        if (m_memoryMap->m_mapa.m_camposvariables[i].TipoCampo==QString("%1").arg(TXT_CAMPO_TIPO_TEXTO)){
            m_memoryMap->m_mapa.m_camposvariables[i].TipoLetraCampo=FicheroIni.value(TXT_FUENTE_CAMPO,"Arial").toString();
            m_memoryMap->m_mapa.m_camposvariables[i].TamanoFuenteCampo=FicheroIni.value(TXT_TAMANO_CAMPO,12).toInt();
            m_memoryMap->m_mapa.m_camposvariables[i].EstiloLetraCampo=FicheroIni.value(TXT_ESTILO_CAMPO,"Normal").toString();
        }

        //Caso de que el campo seade tipo contador
        if (m_memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
            m_memoryMap->m_mapa.m_camposvariables[i].LimiteInferior=FicheroIni.value(TXT_CONTADOR_LIMITE_INFERIOR,0).toInt();
            m_memoryMap->m_mapa.m_camposvariables[i].LimiteSuperior=FicheroIni.value(TXT_CONTADOR_LIMITE_SUPERIOR,0).toInt();
            m_memoryMap->m_mapa.m_camposvariables[i].ValorInicial=FicheroIni.value(TXT_CONTADOR_VALOR_INICIAL,0).toInt();
            m_memoryMap->m_mapa.m_camposvariables[i].Incremento=FicheroIni.value(TXT_CONTADOR_INCREMENTO,0).toInt();
        }

        if (m_memoryMap->m_mapa.m_variables.NumColores>1){
            m_memoryMap->m_mapa.m_campos[i].color=FicheroIni.value(TXT_COLOR_CAMPO,1).toInt();
        }

        for (int j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ESTADO_CAMPO);
            dummy_string2=FicheroIni.value(dummy_string,1024).toString();
            if (dummy_string2==TXT_ON) m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz=1;
            else m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OnOffMatriz=0;

            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_X_CAMPO);
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionXCampo=FicheroIni.value(dummy_string,1024).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COORDENADA_Y_CAMPO);
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].PosicionYCampo=FicheroIni.value(dummy_string,1024).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_ORIENTACION_CAMPO);
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].OrientacionCampo=FicheroIni.value(dummy_string,1024).toInt();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FILAS_MATRIZ_CAMPO);
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo=FicheroIni.value(dummy_string,1024).toInt();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_COLUMNAS_MATRIZ_CAMPO);
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo=FicheroIni.value(dummy_string,1024).toInt();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_FILAS_MATRIZ_CAMPO);
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqFilasMatrizCampo=FicheroIni.value(dummy_string,1024).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_FREQ_COLUMNAS_MATRIZ_CAMPO);
            m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FreqColumnasMatrizCampo=FicheroIni.value(dummy_string,1024).toDouble();
            dummy_string=QString("M%1_%2").arg(j+1).arg(TXT_EXCEPCIONES_MATRIZ_CAMPO);
            dummy_string2=FicheroIni.value(dummy_string,"").toString();
            strncpy(m_memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ExcepcionesMatrizCampo,dummy_string2.toAscii().data(),CAMPOS_LEN);
        }

        FicheroIni.endGroup();
    }

    FicheroIni.beginGroup(TXT_ORIGEN);
    m_memoryMap->m_mapa.m_variables.PuntoInicialX=FicheroIni.value(TXT_COORDENADA_X_ORIGEN,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.PuntoInicialY=FicheroIni.value(TXT_COORDENADA_Y_ORIGEN,1024).toDouble();
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_COMPOSICION);
    m_memoryMap->m_mapa.m_variables.WebWidth=FicheroIni.value(TXT_WEBWIDTH_COMPOSICION,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.WebHeight=FicheroIni.value(TXT_WEBHEIGHT_COMPOSICION,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas=FicheroIni.value(TXT_FILAS_COMPOSICION,1024).toInt();
    m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas=FicheroIni.value(TXT_COLUMNAS_COMPOSICION,1024).toInt();
    m_memoryMap->m_mapa.m_variables.SeparacionY=FicheroIni.value(TXT_SEPARACION_FILAS_COMPOSICION,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.SeparacionX=FicheroIni.value(TXT_SEPARACION_COLUMNAS_COMPOSICION,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.AnguloPlantilla=FicheroIni.value(TXT_ANGULO_COMPOSICION,1024).toInt();
    m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvio=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    FicheroIni.endGroup();


    FicheroIni.beginGroup(TXT_MACULA_IZQDA);
    dummy_string=FicheroIni.value(TXT_ESTADO_MACULA,1024).toString();
    if (dummy_string==TXT_ON) m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=1;
    else m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff=0;
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaX=FicheroIni.value(TXT_COORDENADA_X_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaY=FicheroIni.value(TXT_COORDENADA_Y_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaAncho=FicheroIni.value(TXT_ANCHO_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaAlto=FicheroIni.value(TXT_ALTO_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones=FicheroIni.value(TXT_REPETICIONES_MACULA,1024).toInt();
    m_memoryMap->m_mapa.m_variables.MaculaIzqdaFrecuencia=FicheroIni.value(TXT_FRECUENCIA_MACULA,1024).toDouble();
    if (m_memoryMap->m_mapa.m_variables.NumColores>1){
        m_memoryMap->m_mapa.m_variables.MaculaIzqdaColor=FicheroIni.value(TXT_COLOR_MACULA,1024).toInt();
    }
    FicheroIni.endGroup();

    FicheroIni.beginGroup(TXT_MACULA_DCHA);
    dummy_string=FicheroIni.value(TXT_ESTADO_MACULA,1024).toString();
    if (dummy_string==TXT_ON) m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff=1;
    else m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff=0;
    m_memoryMap->m_mapa.m_variables.MaculaDchaX=FicheroIni.value(TXT_COORDENADA_X_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaDchaY=FicheroIni.value(TXT_COORDENADA_Y_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaDchaAncho=FicheroIni.value(TXT_ANCHO_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaDchaAlto=FicheroIni.value(TXT_ALTO_MACULA,1024).toDouble();
    m_memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones=FicheroIni.value(TXT_REPETICIONES_MACULA,1024).toInt();
    m_memoryMap->m_mapa.m_variables.MaculaDchaFrecuencia=FicheroIni.value(TXT_FRECUENCIA_MACULA,1024).toDouble();
    if (m_memoryMap->m_mapa.m_variables.NumColores>1){
        m_memoryMap->m_mapa.m_variables.MaculaDchaColor=FicheroIni.value(TXT_COLOR_MACULA,1024).toInt();
    }
    FicheroIni.endGroup();

    if (m_memoryMap->m_mapa.m_variables.NumColores>1){
        dummy_string1=QString("%1").arg(m_memoryMap->m_mapa.m_variables.TipoColor);
        dummy_string2=QString("%1").arg(SC_PLENOS);
        if (dummy_string1==dummy_string2){
            for (int i=0;i<m_memoryMap->m_mapa.m_variables.NumColores;i++){
                dummy_string=QString("%1%2").arg(TXT_COLOR).arg(i+1);
                qDebug() << dummy_string;
                FicheroIni.beginGroup(dummy_string);
                m_memoryMap->m_mapa.m_colores[i].x_mm=FicheroIni.value(TXT_COORDENADA_X_COLOR,1024).toDouble();
                m_memoryMap->m_mapa.m_colores[i].y_mm=FicheroIni.value(TXT_COORDENADA_Y_COLOR,1024).toDouble();
                FicheroIni.endGroup();
            }
        }
    }

    */
}


void ImgFixGenerator::CargaPlantilla(){

    unsigned char indice_color;
    QString dumy;

    Convert();

    if (m_memoryMap->m_mapa.m_variables.NumColores==1){
        PixmapUnitario[0].load(m_memoryMap->m_mapa.m_variables.NombreRutaBmp,"BMP",Qt::MonoOnly);
    }
    else{
        for(indice_color=0;indice_color<NUM_COLORES;indice_color++){
            QString dummy=QString(m_memoryMap->m_mapa.m_variables.NombreRutaBmp);
            dummy.replace(QString(".bmp"),QString("_C%1.bmp").arg(indice_color+1));
            PixmapUnitario[indice_color].load(dummy,"BMP",Qt::MonoOnly);
        }
    }


}

//Crea cada una de las imagenes que forma la impresión
void ImgFixGenerator::CreaImagenUnitaria_ColoresPlenos(unsigned int indice){

    unsigned char indice_color;
    unsigned char tamano_fuente_campo;
    int i,j;
    unsigned int Xtemp,Ytemp;
    unsigned int Xotemp,Yotemp;
    unsigned int dix,diy;
    unsigned int celda;
    QString celda_s;
    QString excepciones;
    bool esunaexcepcion;
    QFont mifont;
    int indice_estilo;
    int peso_campo;
    QStringList lista_excepciones;
    QString Vtemp;

    for(indice_color=0;indice_color<NUM_COLORES;indice_color++){
        QPixmap pic=PixmapUnitario[indice_color];
        QPainter p(&pic);
        p.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
        p.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
        for (i=0;i<NUM_CAMPOS;i++){
            if ((m_memoryMap->m_mapa.m_campos[i].OnOffCampo==0x01) &&
                    (m_memoryMap->m_mapa.m_campos[i].color==(indice_color+1))) {
                for (j=0;j<NUM_MATRIZ_POR_CAMPO;j++){
                    if (m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OnOffMatriz==0x01){
                        lista_excepciones.clear();
                        excepciones=QString(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ExcepcionesMatrizCampo);
                        lista_excepciones=excepciones.split(",");
                        Xotemp=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionXCampo);
                        Yotemp=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].PosicionYCampo);
                        dix=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqColumnasMatrizCampo);
                        diy=(unsigned int) (FACTOR_PIXELS_MM*m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FreqFilasMatrizCampo);
                        Vtemp=QString(m_memoryMap->m_mapa.m_campos[i].ValorActualCampo);
                        tamano_fuente_campo=(unsigned int) (FACTOR_PIXELS_PUNTO*m_memoryMap->m_mapa.m_campos[i].TamanoFuenteCampo);
                        indice_estilo=EstiloFuente.indexOf((QString(m_memoryMap->m_mapa.m_campos[i].EstiloLetraCampo)));
                        peso_campo=EstiloFuenteQt.at(indice_estilo);
                        mifont.setPixelSize(tamano_fuente_campo);
                        mifont.setStyleStrategy(QFont::PreferAntialias);
                        mifont.setFamily(m_memoryMap->m_mapa.m_campos[i].TipoLetraCampo);
                        mifont.setWeight(peso_campo);
                        p.setFont(mifont);
                        Ytemp=Yotemp;
                        celda=1;
                        for (unsigned int yy=0;yy<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].FilasMatrizCampo;yy++){
                            Xtemp=Xotemp;
                            for (unsigned int xx=0;xx<m_memoryMap->m_mapa.m_campos[i].m_matrices[j].ColumnasMatrizCampo;xx++){
                                celda_s=QString("%1").arg(celda);
                                esunaexcepcion=lista_excepciones.contains(celda_s,Qt::CaseInsensitive);
                                if (esunaexcepcion==false){
                                    p.save();
                                    p.translate(Xtemp,Ytemp+tamano_fuente_campo);
                                    p.rotate(m_memoryMap->m_mapa.m_campos[i].m_matrices[j].OrientacionCampo);
                                    p.drawText(0,0,Vtemp);
                                    p.restore();
                                }
                                Xtemp+=dix;
                                celda++;
                            }
                            Ytemp+=diy;
                        }
                    }
                }
            }
        }


        QImage tmpimage=pic.toImage().convertToFormat(QImage::Format_RGB32,Qt::ColorOnly);
        QPixmap dummyPixmap=QPixmap::fromImage(tmpimage).transformed(QTransform().rotate(m_memoryMap->m_mapa.m_variables.AnguloPlantilla,Qt::ZAxis));
        //dummyPixmap.save(QString("C:\\color%1.bmp").arg(indice_color+1));
        pixMapArray[indice_color].append(dummyPixmap);

        p.end();

    }

}



void ImgFixGenerator::CreaImagenEnviar_ColoresPlenos(){

    unsigned char indice_color;
    unsigned int i,j;
    unsigned int x;
    unsigned int y;
    int indice;


    //unsigned int canvas_width=m_NumCabezales;
    unsigned int canvas_width=m_memoryMap->m_mapa.m_variables.DefaultNumHeads*1000;

    unsigned int wh;
    unsigned int ipef,ipec;
    unsigned int x0,y0;
    unsigned int dix,diy;

    double ww_mm,wh_mm;
    double x0_mm,y0_mm;
    double dix_mm,diy_mm;

    QVector<QRgb> table( 256 );
    for( int i = 0; i < 256; ++i ){
        table[i] = qRgb(i,i,i);
    }

    ww_mm=m_memoryMap->m_mapa.m_variables.WebWidth;
    wh_mm=m_memoryMap->m_mapa.m_variables.WebHeight;

    dix_mm=m_memoryMap->m_mapa.m_variables.SeparacionX;
    diy_mm=m_memoryMap->m_mapa.m_variables.SeparacionY;

    //fp=m_memoryMap->m_mapa.m_variables.FilasPlantilla;
    //cp=m_memoryMap->m_mapa.m_variables.ColumnasPlantilla;

    if ((m_memoryMap->m_mapa.m_variables.AnguloPlantilla!=0) &&
            (m_memoryMap->m_mapa.m_variables.AnguloPlantilla!=90) &&
            (m_memoryMap->m_mapa.m_variables.AnguloPlantilla!=180) &&
            (m_memoryMap->m_mapa.m_variables.AnguloPlantilla!=270)){
        m_memoryMap->m_mapa.m_variables.AnguloPlantilla=0;
    }

    if ((m_memoryMap->m_mapa.m_variables.AnguloPlantilla==270) || (m_memoryMap->m_mapa.m_variables.AnguloPlantilla==90)){
        //cp=m_memoryMap->m_mapa.m_variables.FilasPlantilla;
        //fp=m_memoryMap->m_mapa.m_variables.ColumnasPlantilla;
    }

    ipef=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;
    ipec=m_memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;
    x0_mm=m_memoryMap->m_mapa.m_variables.PuntoInicialX;
    y0_mm=m_memoryMap->m_mapa.m_variables.PuntoInicialY;


    wh=(unsigned int) (FACTOR_PIXELS_MM*wh_mm);

    dix=(unsigned int) (FACTOR_PIXELS_MM*dix_mm);
    diy=(unsigned int) (FACTOR_PIXELS_MM*diy_mm);

    //dix-fp;
    //diy-cp;

    x0=(unsigned int) (FACTOR_PIXELS_MM*x0_mm);
    y0=(unsigned int) (FACTOR_PIXELS_MM*y0_mm);


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

    unsigned int mi_xo;
    unsigned int mi_yo;
    unsigned int mi_x;
    unsigned int mi_y;
    unsigned int mi_ancho;
    unsigned int mi_alto;
    unsigned int md_xo;
    unsigned int md_yo;
    unsigned int md_x;
    unsigned int md_y;
    unsigned int md_ancho;
    unsigned int md_alto;
    unsigned int mi_frecuencia;
    unsigned int md_frecuencia;
    unsigned int num_mi;


    num_mi=m_memoryMap->m_mapa.m_variables.MaculaIzqdaRepeticiones;
    //m_memoryMap->m_mapa.m_variables.MaculaDchaRepeticiones;

    mi_xo=(unsigned int) (FACTOR_PIXELS_MM*mi_x_mm);
    mi_yo=(unsigned int) (FACTOR_PIXELS_MM*mi_y_mm);
    mi_ancho=(unsigned int) (FACTOR_PIXELS_MM*mi_ancho_mm);
    mi_alto=(unsigned int) (FACTOR_PIXELS_MM*mi_alto_mm);
    mi_frecuencia=(unsigned int) (FACTOR_PIXELS_MM*mi_frecuencia_mm);

    md_xo=(unsigned int) (FACTOR_PIXELS_MM*md_x_mm);
    md_yo=(unsigned int) (FACTOR_PIXELS_MM*md_y_mm);
    md_ancho=(unsigned int) (FACTOR_PIXELS_MM*md_ancho_mm);
    md_alto=(unsigned int) (FACTOR_PIXELS_MM*md_alto_mm);
    md_frecuencia=(unsigned int) (FACTOR_PIXELS_MM*md_frecuencia_mm);

    QPixmap dummy;

    for(indice_color=0;indice_color<NUM_COLORES;indice_color++){
        QPixmap pic(canvas_width,wh);
        QPainter p(&pic);   // create a QPainter for it
        p.setPen(m_memoryMap->m_mapa.m_colores[indice_color].valor);
        pic.fill(Qt::white);
        p.drawPixmap(0,0,dummy); // and draw your original pixmap on it

        //Para el color indice_color
        x=x0;
        y=y0;
        indice=0;

        for(i=0;i<ipef;i++){
            for(j=0;j<ipec;j++){
                QPixmap pixmap=pixMapArray[indice_color].at(indice);
                p.drawPixmap(x,y, pixmap);
                x=x+dix;//cp+sx;
                indice++;
            }
            x=x0;
            y=y+diy;//fp+sy;
        }

        QColor Color(m_memoryMap->m_mapa.m_colores[indice_color].valor);
        QBrush b(Color,Qt::SolidPattern);

        if (m_memoryMap->m_mapa.m_variables.MaculaIzqdaOnOff==1){
            mi_x=mi_xo;
            mi_y=mi_yo;
            for (unsigned int n=0;n<num_mi;n++){
                if (m_memoryMap->m_mapa.m_variables.MaculaIzqdaColor==(indice_color+1)) p.fillRect(mi_x,mi_y,mi_ancho,mi_alto,b);
                mi_y+=mi_frecuencia;
            }
        }

        if (m_memoryMap->m_mapa.m_variables.MaculaDchaOnOff==1){
            md_x=md_xo;
            md_y=md_yo;
            for (unsigned int n=0;n<num_mi;n++){
                if (m_memoryMap->m_mapa.m_variables.MaculaDchaColor==(indice_color+1)) p.fillRect(md_x,md_y,md_ancho,md_alto,b);
                md_y+=md_frecuencia;
            }
        }

        QPixmap rotatedpixmap=pic.transformed(QTransform().rotate(0,Qt::ZAxis));
        m_memoryMap->m_Imagen=rotatedpixmap;
        QImage rotatedimage=rotatedpixmap.toImage();

        genImageToSend[indice_color]=rotatedimage.convertToFormat(QImage::Format_Indexed8,table,Qt::ColorOnly);
        genImageToSend[indice_color].setDotsPerMeterX(14174);
        genImageToSend[indice_color].setDotsPerMeterY(14174);

        QString nombreruta_ini=QString(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
        nombreruta_ini.replace(QString(".ini"),QString("_C%1_comp.bmp").arg(indice_color+1));
        genImageToSend[indice_color].save(nombreruta_ini,"BMP");

        p.end();

    }


    QMessageBox::information(0,"INFO","Imagen generada correctamente",QMessageBox::Ok);
    QString registro;
    registro=QString("%1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni);
    //Insertar_Evento(CREA_COMPOSICION,registro);

}
