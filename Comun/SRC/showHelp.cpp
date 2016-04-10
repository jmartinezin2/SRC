/** @addtogroup showHelp
 *  @{*/
/** @file showHelp.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: showHelp.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#include "showHelp.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDateTimeEdit>
#include <QSessionManager>
#include <QApplication>
#include <Windows.h>
#include <QFontComboBox>
#include <assert.h>
#include <QHash>
#include <QPixmap>
#include <QFontComboBox>
#include <QDebug>
#include "globals.h"


#define POS_PATH_DBHELP     0
#define POS_CAPTION_DBHELP  1
#define POS_NIVEL_DBHELP    2
#define POS_TAGS_DBHELP     3

#define NUM_CAMPOS_DBHELP   4


showHelp *showHelp::mInstance  = NULL;

/** Singleton*/
showHelp *showHelp::request(QWidget *parent)
{
    if(mInstance == NULL){
        mInstance = new showHelp(parent);
    }
    return mInstance;
}

/** Constructor*/
showHelp::showHelp(QWidget *parent)
    : ui(new Ui::helpUI)
{
    ui->setupUi(this);

    connect(ui->m_Index,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(muestraElementoSeleccionado()));
    connect(ui->m_viewItem,SIGNAL(clicked()),this,SLOT(muestraElementoSeleccionado()));
    connect(ui->m_Search,SIGNAL(clicked()),this,SLOT(realizarBusqueda()));
    connect(ui->m_Results,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(muestraResultadoSeleccionado()));
    connect(ui->m_viewResults,SIGNAL(clicked()),this,SLOT(muestraResultadoSeleccionado()));

    connect(ui->m_Right,SIGNAL(clicked()),this,SLOT(avanzarNivel()));
    connect(ui->m_Left,SIGNAL(clicked()),this,SLOT(retrocederNivel()));

    connect(ui->m_Right_Results,SIGNAL(clicked()),this,SLOT(avanzarNivelResultados()));
    connect(ui->m_Left_Results,SIGNAL(clicked()),this,SLOT(retrocederNivelResultados()));

    //initVariables();

    QLabel *dummyLabel;
    dummyLabel=ShowMensaje("Por favor espere");

    abrirBaseDatos();
    mostrarPaginaDefault();

    delete(dummyLabel);

    ui->tabControl->setCurrentIndex(0);

}

/** Destructor*/
showHelp::~showHelp(){

    Salir();

}

/** Salida de la ui*/
void showHelp::Salir(){

    miDataBaseHelp.close();
    this->deleteLater();

}

/** Inicializacion de variables*/
void showHelp::initVariables(){

    /*numCampos=NUM_CAMPOS_DBHELP;

    AbsolutePathDbHelp.clear();
    AbsolutePathDbHelp=DevuelveRuta(AbsolutePathDbLog);
    AbsolutePathDbHelp.append("\\help.sqlite");

    AbsolutePathHelpFiles.clear();
    AbsolutePathHelpFiles=DevuelveRuta(AbsolutePathDbLog);
    AbsolutePathHelpFiles.replace("Database","help",Qt::CaseInsensitive);
    AbsolutePathHelpFiles.append("\\");

    indice.clear();
    resultados.clear();*/
}


/** Apertura de base de datos*/
void showHelp::abrirBaseDatos(){

   /* bool bRet;

    if (miDataBaseHelp.open(AbsolutePathDbHelp,"ConexionHelp")!=true){
        //Error
    }
    else{
        bRet=miDataBaseHelp.exec("select * from helpContents order by Nivel");
        if (bRet){
            cargaIndice();
        }
    }*/

}


/** Muestra en pantalla todo el contenido de la base de datos de ayuda*/
void showHelp::cargaIndice(){

    /*ui->m_Index->clear();

    QString tmp,tmpCaption,tmpSpaces,tmpNivel;
    int dummy_int;

    if (miDataBaseHelp.m_Tabla.count()>0){
        for (int index=0;index<miDataBaseHelp.m_Tabla.count();index=index+numCampos){
            tmpCaption=miDataBaseHelp.m_Tabla.at(index+POS_CAPTION_DBHELP);
            tmpNivel=miDataBaseHelp.m_Tabla.at(index+POS_NIVEL_DBHELP);
            indice.append(tmpNivel);
            dummy_int=tmpNivel.count(".");
            tmpSpaces.clear();
            for (int i=0;i<dummy_int;i++) tmpSpaces.append("  ");
            tmp=QString("%1%2").arg(tmpSpaces).arg(tmpCaption);
            ui->m_Index->addItem(tmp);
        }
    }*/

}


/** Muestra en pantalla el resultado seleccionado de la busqueda realizada  */
void showHelp::muestraResultadoSeleccionado(){

    /*QString captionSeleccionado;
    int elementoSeleccionado;

    if (ui->m_Results->count()>0){
        elementoSeleccionado=ui->m_Results->currentRow();
        captionSeleccionado=resultados.at(elementoSeleccionado);
        muestraElemento(captionSeleccionado);
    }
    else{
        mostrarPaginaDefault();
    }*/

}

/** Muestra en pantalla el elemento seleccionado del índice  */
void showHelp::muestraElementoSeleccionado(){

   /* QString captionSeleccionado;
    int elementoSeleccionado;

    if (ui->m_Index->count()>0){
        elementoSeleccionado=ui->m_Index->currentRow();
        captionSeleccionado=indice.at(elementoSeleccionado);
        muestraElemento(captionSeleccionado);
    }
    else{
        mostrarPaginaDefault();
    }*/
}


/** Muestra en pantalla el elemento seleccionado*/
void showHelp::muestraElemento(QString nivel){

    /*QString fileSeleccionado;
    QString rutaTotal;
    QUrl docActual;

    QLabel *dummyLabel;
    dummyLabel=ShowMensaje("Por favor espere");

    fileSeleccionado=devuelveArchivoMostrar(nivel);

    rutaTotal=AbsolutePathHelpFiles;
    rutaTotal.append(fileSeleccionado);

    bool fileExists=QFile::exists(rutaTotal);

    if (fileExists){
        docActual.clear();
        docActual.setPath(QString("%1").arg(rutaTotal));
        ui->m_Contents->load(docActual);
        ui->m_Contents->show();
    }
    else{
        mostrarPaginaNoEncontrada();
    }

    delete (dummyLabel);*/
}


/** Devuelve el nombre+ruta del fichero htm que hay que mostrar  */
QString showHelp::devuelveArchivoMostrar(QString nivel){

    /*QString sql;
    bool bRet;
    QString tmp;

    tmp.clear();

    sql=QString("select Path from helpContents where Nivel='%1' order by Nivel").arg(nivel);

    miDataBaseHelp.m_Tabla.clear();

    bRet=miDataBaseHelp.exec(sql.toAscii().data());

    if (bRet){
        if (miDataBaseHelp.m_Tabla.count()>0){
           tmp=miDataBaseHelp.m_Tabla.at(0);
        }
    }

    return tmp;*/
}


/** Inicia una búsqueda en la base de datos */
void showHelp::realizarBusqueda(){

    QString texto;

    texto=ui->m_TagToSearch->toPlainText();

    ui->m_Results->clear();

    resultados.clear();

    busquedaTag(texto);

}



/** Realiza una búsqueda en la base de datos */
void showHelp::busquedaTag(QString nivel){

    QString sql;
    bool bRet;
    QString tmp,tmpNivel;

    tmp.clear();

    sql=QString("select * from helpContents where Tags like '\%%1\%' order by Nivel").arg(nivel);

    miDataBaseHelp.m_Tabla.clear();

    bRet=miDataBaseHelp.exec(sql.toAscii().data());

    if (bRet){
        for (int index=0;index<miDataBaseHelp.m_Tabla.count();index=index+numCampos){
            tmp=miDataBaseHelp.m_Tabla.at(index+POS_CAPTION_DBHELP);
            tmpNivel=miDataBaseHelp.m_Tabla.at(index+POS_NIVEL_DBHELP);
            ui->m_Results->addItem(tmp);
            resultados.append(tmpNivel);
        }
    }

}



/** Avanza un nivel en el índice */
void showHelp::avanzarNivel(){

    int indiceActual;
    int indiceMostrar;
    int numElementos;

    numElementos=ui->m_Index->count();
    indiceActual=ui->m_Index->currentRow();

    if (indiceActual<(numElementos-1)){
        indiceMostrar=indiceActual+1;
        ui->m_Index->setCurrentRow(indiceMostrar);
        muestraElementoSeleccionado();
    }

}


/** Retrocede un nivel en el índice */
void showHelp::retrocederNivel(){

    int indiceActual=0;
    int indiceMostrar=0;

    indiceActual=ui->m_Index->currentRow();

    if (indiceActual>0){
        indiceMostrar=indiceActual-1;
        ui->m_Index->setCurrentRow(indiceMostrar);
        muestraElementoSeleccionado();
    }

}



/** Avanza un nivel en los resultados */
void showHelp::avanzarNivelResultados(){

    int indiceActual=0;
    int indiceMostrar=0;
    int numElementos=0;

    numElementos=ui->m_Results->count();
    indiceActual=ui->m_Results->currentRow();

    if (indiceActual<(numElementos-1)){
        indiceMostrar=indiceActual+1;
        ui->m_Results->setCurrentRow(indiceMostrar);
        muestraResultadoSeleccionado();
    }

}


/** Retrocede un nivel en los resultados */
void showHelp::retrocederNivelResultados(){

    int indiceActual=0;
    int indiceMostrar=0;

    indiceActual=ui->m_Results->currentRow();

    if (indiceActual>0){
        indiceMostrar=indiceActual-1;
        ui->m_Results->setCurrentRow(indiceMostrar);
        muestraResultadoSeleccionado();
    }

}


/** Muestra la página por defecto */
void showHelp::mostrarPaginaDefault(){
    QUrl docActual;
    QString paginaInicio;

    paginaInicio=QString("%1\\Index.htm").arg(AbsolutePathHelpFiles);
    docActual.setPath(paginaInicio);
    ui->m_Contents->load(docActual);
    ui->m_Contents->show();
}


/** Muestra la página en caso de no existir la pedida */
void showHelp::mostrarPaginaNoEncontrada(){
    QUrl docActual;
    QString paginaInicio;

    paginaInicio=QString("%1\\PageNotFound.htm").arg(AbsolutePathHelpFiles);
    docActual.setPath(paginaInicio);
    ui->m_Contents->load(docActual);
    ui->m_Contents->show();
}
