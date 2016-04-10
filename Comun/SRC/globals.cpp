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

#include <QDebug>
#include <QCoreApplication>
#include <QApplication>

//#include "comthread.h"
//#include "serialport.h"
#include "MemoryMap.h"
#include "globals.h"
//#include "GestorDB_ODBC.h"
#include "GestorDB_SQLITE.h"
#include "inkjetdebug.h"
#include <QFileDialog>
#include <QString>
#include <QChar>
#include <QPixmap>
#include <QImage>
#include <QHash>
#include <QMessageBox>
#include <QStringList>
#include <QSettings>
#include <QProcess>
#include <QVariantList>
//#include <QAxObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QWidget>
#include <QUuid>
#include <QXmlReader>
#include <QDomDocument>
#include <QCryptographicHash>
#include <QFile>
#include <QDesktopWidget>
#include <QMap>
#include "XCL_Library.h"
#include <QDebug>
#include <QTranslator>
#include "gsrenderengine.h"
#include "qmath.h"
#include "in2log.h"

#include "windows.h"

#ifndef NO_OPC
    #include "opcautomation.h"
#endif

//Puntero a la aplicacion principal
static QWidget *MainWindow;
static QString gInkjetPath;

//Mapa de memoria unico de la aplicacion
//MemoryMap *memoryMap;
//Clase principal
QWidget *mainClass;
QString AbsolutePathDbLog;
//QString AbsolutePathDbColaImpresion;

//Estructura de datos para manejar la dll de Scorpion parael XUSB de Xaar
//hay que mentener una copia de esta estructura por cabezal
//Puede haber varios XUSB conectados
//UpdatedPrintDataParameters MisParametrosXUSB[NUM_XUSB][NUM_PH_PER_XUSB];

unsigned char flag_Hydra_ok;
unsigned char flag_Hydra_ok_ant;

unsigned char flag_Maquina_ok;
unsigned char flag_Maquina_ok_ant;

unsigned char flag_comunicacion_Hydra_ok;
unsigned char flag_temperatura_Hydra_ok;
unsigned char flag_nivel_Hydra_ok;
unsigned char flag_fill_Hydra_ok;

unsigned char flag_led_ok;

unsigned char codigo_error;

unsigned char tecla_XUSB;

unsigned char flag_start_Hydra_realizado;
unsigned char flag_shutdown_Hydra_realizado;
unsigned char flag_restart_Hydra_realizado;
unsigned char flag_descanso_Hydra;

unsigned char flag_cambio_user_mode;

HWND HandleXUSB;
bool VentanaXUSBEncontrada;
int NumeroVentanas;
unsigned char flag_esperar_PrintComplete;

unsigned char flag_led_Maquina_rojo;
unsigned char flag_led_Lampara_rojo;


extern QStringList TipoUsuarios;

unsigned char index_nivel_Hydra;

unsigned char contador_comunicacion_Hydra_ko;

#ifndef NO_OPC
using namespace OPCAutomation;
#endif

QStringList EstiloFuente=(QStringList()<<"Light"<<"Normal"<<"DemiBold"<<"Bold"<<"Black");
QList<int> EstiloFuenteQt=(QList<int>()<<QFont::Light<<QFont::Normal<<QFont::DemiBold<<QFont::Bold<<QFont::Black);
QStringList NombreColumnasTablaLog=(QStringList()<<"Fecha"<<"Usuario"<<"Evento"<<"Valor");



unsigned char MantenimientoPendiente;
int NumeroColumnasTablaMantenimiento=4;
int PosicionTiempoSesion=2;

unsigned char flag_error_iniciando[MAXHYDRAS]={0,0};
unsigned char flag_error_iniciando_hydra=0;

unsigned char flag_referencia_velocidad_alcanzada=1;


#define  ROOTUSERNAMEENCRYPTED "ed0225b8b69a7dd03470f0f5f66bfe1ef0e06908"	//Nombre de usuario encriptado de Superusuario
#define  ROOTPASSWORDENCRYPTED  "61768db8d1a38f1c16d3e6eea812ef423c739068"	//Contraseña encriptada de Superusuario


// Implementacion de sleep
void _Sleep(int iTimeout){
    Sleep(iTimeout);
}

//Limpia la cola de mensajes
void CleanQueueMsg(){
    MSG message;
    while (PeekMessage( &message, NULL, 0, 0, PM_REMOVE)!=0){
            ::TranslateMessage(&message);
    ::DispatchMessage(&message);
    }
}







void GeneracionRutaPlantilla(){

    QString nombre_ruta_ini;
    QString nombre_ini;
    QString ruta_ini;
    QString nombre_ruta_pdf;
    QString nombre_pdf;
    QString ruta_pdf;
    QString nombre_ruta_bmp;
    MemoryMap *memoryMap=MemoryMap::request(0);

    nombre_ruta_ini=QString(memoryMap->m_mapa.m_variables.NombreRutaIni);
    nombre_ini=QString(memoryMap->m_mapa.m_variables.NombreIni);
    ruta_ini=QString(memoryMap->m_mapa.m_variables.RutaIni);

    //Para poner que el nombre/ruta del pdf sea el que aparezca en el *.INI
    //hay que poner el siguiente bloque
    nombre_pdf=QString(memoryMap->m_mapa.m_variables.NombrePdf);
    ruta_pdf=QString(memoryMap->m_mapa.m_variables.RutaPdf);
    nombre_ruta_pdf=QString("%1\\%2").arg(ruta_pdf).arg(nombre_pdf);
    memoryMap->m_mapa.m_variables.NombreRutaPdf=nombre_ruta_pdf;

    nombre_ruta_bmp=nombre_ruta_ini;
    nombre_ruta_bmp.replace(QString(".ini"), QString(".bmp"));
    memoryMap->m_mapa.m_variables.NombreRutaBmp=nombre_ruta_bmp;
    memoryMap->m_mapa.m_variables.RutaBmp=ruta_ini;
}


void deleteTempGSFile(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    QString dummy_bmp;
    dummy_bmp=QString("%1\\dummy.bmp").arg(memoryMap->m_mapa.m_variables.RutaBmp);
    QFile::remove(dummy_bmp);
}




//Esta funcion la dejamos sólo para el caso de que el equipo de fluidica sea el Hydra de Xaar



unsigned char EscaladoDeImagenEnQLabel(int lw, int lh, int iw, int ih){

    //unsigned char escalado;
    int dif_w=lw-iw;
    int dif_h=lh-ih;

    /*unsigned int cociente_w;
    unsigned int cociente_h;*/

    unsigned char escalado;
    unsigned int cociente_w;
    unsigned int cociente_h;


    cociente_w=(unsigned int)((1000*iw)/lw);
    cociente_h=(unsigned int)((1000*ih)/lh);

    if ( (dif_w>=0) && (dif_h>=0)){
        escalado=IMAGEN_SINESCALAR;
    }
    else if (cociente_h>cociente_w){
        escalado=IMAGEN_S2H;
    }
    else{ // if (cociente_h<=cociente_w){
        escalado=IMAGEN_S2W;
    }

    return (escalado);
}


/** Testea el supersuser*/
bool TestSuperUser(){
    MemoryMap *memoryMap=MemoryMap::request(0);
#if QT_VERSION >= 0x050000
    QByteArray usuario_dummy=memoryMap->getUser().toLatin1();
    QByteArray RootUsernameEncrypted_dummy=QString("%1").arg(ROOTUSERNAMEENCRYPTED).toLatin1();
    QByteArray password_dummy=memoryMap->getPassword().toLatin1();
    QByteArray RootPasswordEncrypted_dummy=QString("%1").arg(ROOTPASSWORDENCRYPTED).toLatin1();
#else
    QByteArray usuario_dummy=memoryMap->getUser().toAscii();
    QByteArray RootUsernameEncrypted_dummy=QString("%1").arg(ROOTUSERNAMEENCRYPTED).toAscii();
    QByteArray password_dummy=memoryMap->getPassword().toAscii();
    QByteArray RootPasswordEncrypted_dummy=QString("%1").arg(ROOTPASSWORDENCRYPTED).toAscii();
#endif
    bool bUser;
    bUser=TestHash(usuario_dummy,RootUsernameEncrypted_dummy);
    bool bPassword;
    bPassword=TestHash(password_dummy,RootPasswordEncrypted_dummy);

    if (bUser && bPassword) return true;
    else return false;


}



QByteArray CalculateHash(QByteArray DatosEncriptar){
    QByteArray resultado;
    QCryptographicHash MyHash(QCryptographicHash::Sha1);
    MyHash.addData(DatosEncriptar);
    resultado=MyHash.result().toHex();
    return resultado;
}


bool TestHash(QByteArray DatosEncriptar,QByteArray ClaveEncriptada){

    bool bRet;

    QByteArray DatosEncriptados=CalculateHash(DatosEncriptar);

    if (ClaveEncriptada==DatosEncriptados) bRet=true;
    else bRet=false;

    return bRet;

}


bool HashOk(QByteArray HashLeido,QByteArray DatosEncriptar){
    bool res;
    QByteArray resultado;
    resultado=CalculateHash(DatosEncriptar);
    if (resultado==HashLeido) res=true;
    else res=false;
    return res;
}







//Establece cual es la pantalla principal
void GlobalSetMainWindow(QWidget *pMainWindow){
    MainWindow=pMainWindow;
}
//Muestra la pantalla principal
void GlobalShowMainWindow(){
    if (MainWindow)
        MainWindow->show();
}

//Oculta la pantalla principal
void GlobalHideMainWindow(){
    MainWindow->showMinimized();
}

//Devuelve un puntero a la aplicacion principal
QWidget* GlobalGetMainWindow(){
    return MainWindow;
}

void MostrarInicio(void){
    //Minimizar...hay que emitir señal
    //this->showMinimized();
}

void MostrarAyuda(void){
        QMessageBox::information(0,"Contacto","In2 Printing Solutions\nCarlos Corchón\nTfno 636648974\nwww.in2ps.com",QMessageBox::Ok);
}


QString DevuelveNombre(QString NombreRuta){

    QString dummy;
    QString nombre;

    int posicion_barra;
    int posicion_contrabarra;
    int ultima_posicion;

    dummy=NombreRuta;

    posicion_barra=dummy.lastIndexOf("\\");
    posicion_contrabarra=dummy.lastIndexOf("/");
    if (posicion_contrabarra==-1) posicion_contrabarra=0;
    if (posicion_barra>posicion_contrabarra) ultima_posicion=posicion_barra;
    else ultima_posicion=posicion_contrabarra;

    nombre.clear();
    nombre = dummy.mid(ultima_posicion+1);

    nombre.replace("/","\\");

    return nombre;

}

QString DevuelveRuta(QString NombreRuta){

    QString dummy;
    QString ruta;

    int posicion_barra;
    int posicion_contrabarra;
    int ultima_posicion;

    dummy=NombreRuta;

    posicion_barra=dummy.lastIndexOf("\\");
    posicion_contrabarra=dummy.lastIndexOf("/");
    if (posicion_contrabarra==-1) posicion_contrabarra=0;
    if (posicion_barra>posicion_contrabarra) ultima_posicion=posicion_barra;
    else ultima_posicion=posicion_contrabarra;

    ruta.clear();
    ruta = dummy.left(ultima_posicion);

    ruta.replace("/","\\");

    return ruta;

}

QString DevuelveNombreRutaIni(QString NombreRutaBmp){

    QString dummy;

    int indice=NombreRutaBmp.lastIndexOf("_OF");

    if (indice==-1){
        //No hay orden de fabricación en el nombre
        dummy=NombreRutaBmp;
        dummy=dummy.replace("_C1_comp.bmp",".ini");
    }
    else{
        dummy=NombreRutaBmp.left(indice);
        dummy.append(".ini");
    }
    return dummy;

}


/** Tamaño de la apli*/
void GlobalSetScreenSize(QWidget *p){

    QDesktopWidget d;
    QRect rect=d.screenGeometry();
    p->setMaximumWidth(rect.width());
    p->setMaximumHeight(rect.height());
    Qt::WindowFlags flags=Qt::Window|Qt::CustomizeWindowHint|Qt::WindowTitleHint|Qt::WindowMinimizeButtonHint;
    //this->setWindowFlags(flags|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    p->setWindowFlags(flags);
    p->showMaximized();

}



QString DevuelveExtension(QString NombreRuta){

    QString dummy;
    QString extension;

    int posicion_punto,size;

    dummy=NombreRuta;

    posicion_punto=dummy.lastIndexOf(".",-1,Qt::CaseSensitive);
    size=dummy.length();

    extension.clear();
    extension = dummy.right(size-posicion_punto-1);

    //qDebug() << "Fichero origen: " << dummy;
    //qDebug() << "Extension fichero origen: " << extension;

    return extension;

}


void PruebaTonta(){

    int valor_actual;
    int limite_inferior, limite_superior, valor_inicial, incremento;
    int modulo, modulo_inicial, index1, index2;
    int dummy_index,index,i;


    //La variable es de tipo contador y hay que sacar su valor conforme a valorinicial, incemento...
    limite_inferior=50;
    limite_superior=80;
    valor_inicial=60;
    incremento=2;
    //index_umbral=(int) ((limite_superior-valor_inicial)/incremento);
    if (incremento>=0){
        modulo=(int) ((limite_superior-limite_inferior)/(incremento));
        modulo_inicial=(int) ((limite_superior-valor_inicial)/(incremento));
     }
    else{
        modulo=(int) ((limite_inferior-limite_superior)/incremento);
        modulo_inicial=(int) ((limite_inferior-valor_inicial)/incremento);
    }

    qDebug() << "Modulos: " << modulo << modulo_inicial;

    dummy_index=0;

    for (index=0;index<100;index++){

        qDebug() << "indice: " <<index;

        dummy_index=index*5; //2 equivale a ImpresionesPorEnvio

        for(i=0;i<5;i++){
            if (incremento>0){
                if (dummy_index<=modulo_inicial) valor_actual=valor_inicial+dummy_index*incremento;
                else{
                    index1=dummy_index-modulo_inicial-1;
                    index2=index1%(modulo+1);
                    valor_actual=limite_inferior+index2*incremento;
                }
            }
            else{
                if (dummy_index<=modulo_inicial) valor_actual=valor_inicial+dummy_index*incremento;
                else{
                    index1=dummy_index-modulo_inicial-1;
                    index2=index1%(modulo+1);
                    valor_actual=limite_superior+index2*incremento;
                }
            }
            dummy_index++;
            qDebug() << "valor actual: " <<valor_actual;
        }

    }


}


void Insertar_Horas_Funcionamiento_Lampara(long horas_sesion){

    QString tmp;
    QString fecha;
    QString hora;
    QString fechahora;

    if (horas_sesion>0){

        QDate dummyD=QDate::currentDate();
        fecha=dummyD.toString("yyyy-MM-dd");

        QTime dummyT=QTime::currentTime();
        hora=dummyT.toString();

        fechahora=QString("%1 %2").arg(fecha).arg(hora);

        /*tmp=QString("insert into Mantenimiento ('Equipo','Fecha','TiempoSesion','Usuario') Values ('Lampara','%1','%2','%3')")
                 .arg(fechahora)
                 .arg(horas_sesion)
                 .arg(memoryMap->getUser());*/

        tmp=QString("update Mantenimiento set TiempoSesion='%1' where Equipo='Lampara'").arg(horas_sesion);
        In2Log db;
        db.execSQLNoSave(tmp);
    }
}

void Insertar_Aviso_Tinta(void){


    QString fecha;
    QString hora;
    QString fechahora;
    unsigned char horas_sesion;
    MemoryMap *memoryMap=MemoryMap::request(0);
    QDate dummyD=QDate::currentDate();
    fecha=dummyD.toString("yyyy-MM-dd");

    QTime dummyT=QTime::currentTime();
    hora=dummyT.toString();

    fechahora=QString("%1 %2").arg(fecha).arg(hora);
    horas_sesion=0;

    QString tmp=QString("insert into Mantenimiento ('Equipo','Fecha','TiempoSesion','Usuario') Values ('Tinta','%1','%2','%3')")
             .arg(fechahora)
             .arg(horas_sesion)
             .arg(memoryMap->getUser());

    In2Log db;
    db.execSQLNoSave(tmp);

}


void DiaLimpezaLampara(void){

    int hoy=QDate::currentDate().dayOfWeek();

    if (hoy==Qt::Friday){
        QMessageBox::warning(0,"INFO","Hoy hay que realizar una operación de mantenimiento\na la lámpara de curado UV",QMessageBox::Ok);
        MantenimientoPendiente|=MASCARA_DIA_LAMPARA; //pongo a 1 el bit
    }
    else{
        MantenimientoPendiente&=~MASCARA_DIA_LAMPARA; //pongo a 0 el bit
    }
}


/** Devuelve el nombre de las columnas de la tabla de log*/
QStringList GetNombreColumnasLog(){
    return NombreColumnasTablaLog;
}


int Relacion_VelocidadRampaAceleracion(int Velocidad){

    double Rampa_d;
    int ret;
    //La relacion es: (x=Velocidad;y=Aceleracion)
    // a  0m/min, Intensidad=0%
    // a 25m/min, Intensidad=70%  m=y/x=70/25=2.8
    Rampa_d=(double) (2.8*Velocidad);
    ret=(int) Rampa_d;

    if (ret>100) ret=100;

    return ret;
}

int Relacion_VelocidadRampaDeceleracion(int Velocidad){

    double Rampa_d;
    int ret;
    //La relacion es: (x=Velocidad;y=Aceleracion)
    // a  0m/min, Intensidad=0%
    // a 25m/min, Intensidad=70%  m=y/x=70/25=2.8
    Rampa_d=(double) (2.8*Velocidad);
    ret=(int) Rampa_d;

    if (ret>100) ret=100;

    return ret;
}

int Relacion_VelocidadTiempoBusqueda(int Velocidad){

    double Rampa_tb;
    int ret;
    //La relacion es: (x=Velocidad;y=TiempoBusqueda)
    // a  0m/min, TiempoBusqueda=2s
    // a 25m/min, TiempoBusqueda=5s
    Rampa_tb=(double) (2.8*Velocidad);
    ret=(int) Rampa_tb;

    if (ret>100) ret=100;

    return ret;
}
/** Muestra mensaje en zona central*/
QLabel *ShowMensaje(QString texto){
    QLabel *InfoLabel=new QLabel(texto);
    Qt::WindowFlags flags=Qt::Window;
    QDesktopWidget d;
    InfoLabel->setWindowFlags(flags|Qt::CustomizeWindowHint);
    InfoLabel->setGeometry((d.width()-AUX_INFO_WIDTH)/2,(d.height()-AUX_INFO_HEIGHT)/2,AUX_INFO_WIDTH,AUX_INFO_HEIGHT);
    InfoLabel->setWindowOpacity(0.75);
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    InfoLabel->setFont(font);
    InfoLabel->setAlignment(Qt::AlignVCenter |Qt:: AlignHCenter);
    InfoLabel->show();
    QCoreApplication::processEvents();

    return InfoLabel;
}
/** Registra una función en uso*/
void RegistraFuncion(const QString & qstr){
    //memoryMap->m_DebugLine=qstr;
}
/** Cambia el estilo de la aplicacion*/
void ChangeStyle(int theme){
    QApplication::setStyle("plastique");
    /*QApplication::setStyle("plastique");
    QApplication::setStyle(new InkjetStyle(theme));
    QApplication::setStyle("cleanlooks");*/
}

/** Devuelve la paleta para text-edit-enabled*/
QPalette GetPaletaTextEditEnabled(){
    QPalette paleta_textedit_enabled;
    paleta_textedit_enabled.setColor(QPalette::Base,QColor(R_TEXTEDIT_ON, G_TEXTEDIT_ON, B_TEXTEDIT_ON, A_TEXTEDIT_ON));
    return paleta_textedit_enabled;
}
/** Devuelve la paleta para text-edit-disabled*/
QPalette GetPaletaTextEditDisabled(){
    QPalette paleta_textedit_disabled;
    paleta_textedit_disabled.setColor(QPalette::Base,QColor(R_TEXTEDIT_OFF, G_TEXTEDIT_OFF, B_TEXTEDIT_OFF, A_TEXTEDIT_OFF));
    return paleta_textedit_disabled;
}
/** Guarda la direccion de la apli*/
void SetApplicationDirPath(const QString &appPath){
    gInkjetPath=appPath;
}
/** Devuelve la direccion de la apli*/
QString GetApplicationDirPath(){
    return gInkjetPath;
}

/** Devuelve la BBDD local a partir de una orden de fabircacion*/
QString _GetBBDD(){
    QString rtnName;
    MemoryMap *memoryMap=MemoryMap::request(0);
    if (!memoryMap->m_mapa.m_database.NombreRutaDb.isEmpty()){
        QFile inputFile(memoryMap->m_mapa.m_database.NombreRutaDb);
        if (inputFile.exists()){
            QFileInfo fileInfo(inputFile.fileName());
            QString name(fileInfo.fileName());
            rtnName=QString(memoryMap->m_mapa.m_variables.NombreRutaIni);
            rtnName.remove(".ini");
            rtnName.append(QString("%1").arg(memoryMap->getOrdenAndProducto()));
            rtnName.append(name);
        }
    }
    return rtnName;
}


/** Genera hash que identifica al fichero...lo hace sobre todos sus datos + fecha de creacion + fecha ultima modificacion*/
QByteArray GenerateHashFromFile(const QString &strFile){
    QFile file(strFile);
    QByteArray output;
    QByteArray byteFile;
    if (file.exists()){
        QStringList tmp=strFile.split(".");
        if (tmp.count()==2){
            if (tmp.at(1).compare("mdb")==0){
            /*    miDataBaseCampos.close();
                if (miDataBaseCampos.open(strFile)){
#if QT_VERSION >= 0x050000
                    miDataBaseCampos.exec(QString("select * from DatosImprimir").toLatin1().data());
                    QStringList data=miDataBaseCampos.m_Tabla;
                    if (miDataBaseCampos.exec(QString("select * from Repeticiones").toLatin1().data()))
                        data+=miDataBaseCampos.m_Tabla;
                    if (miDataBaseCampos.exec(QString("select * from FixedData").toLatin1().data()))

#else
                    miDataBaseCampos.exec(QString("select * from DatosImprimir").toAscii().data());
                    QStringList data=miDataBaseCampos.m_Tabla;
                    if (miDataBaseCampos.exec(QString("select * from Repeticiones").toAscii().data()))
                        data+=miDataBaseCampos.m_Tabla;
                    if (miDataBaseCampos.exec(QString("select * from FixedData").toAscii().data()))
#endif
                        data+=miDataBaseCampos.m_Tabla;
                    for (int i=0;i<data.count();i++) byteFile.append(data.at(i));
                    miDataBaseCampos.close();
                }*/
            }
            else{
                file.open(QIODevice::ReadOnly);
                byteFile=file.readAll();
                file.close();
            }
        }
        else{
            file.open(QIODevice::ReadOnly);
            byteFile=file.readAll();
            file.close();

        }
    }
    output=CalculateHash(byteFile);
    return output;
}

/** Genera un fichero .lck a partir del fichero que se le pasa*/
 void GenerateImpresionFileLck(const QString &filename){
     QString path=QString("%1\\Historico").arg(QApplication::applicationDirPath());
     QDir dir;
     dir.mkpath(path);
     path.append(QString("\\%1").arg(filename));
     QString dest=path.left(path.lastIndexOf("."));
     dest.append(".lck");
     QFile destFile(dest);
     if (destFile.open(QIODevice::WriteOnly))
        destFile.close();
 }
 /** Devuelve un fichero .lck a partir del fichero que se le pasa*/
  QString ReturnImpresionFileLck(const QString &filename){
      QString path=QString("%1\\Historico").arg(QApplication::applicationDirPath());
      QDir dir;
      dir.mkpath(path);
      path.append(QString("\\%1").arg(filename));
      QString dest=path.left(path.lastIndexOf("."));
      dest.append(".lck");
      return dest;
  }


  //Crea cada una de las imagenes que forma la impresion
 void PruebaContador(int index){

      QString Vtemp;
      long dummy_index;

      long valor_actual=0;
      //long offset_columna=0;//=VARIABLE_OFFSET_COLUMNA;
      long valor_inicial_fila=0;

      long limite_inferior=10;
      long limite_superior=10000;
      long incremento=1;
      long offset_columna=1000;

      unsigned char i=0,j=0;
      MemoryMap *memoryMap=MemoryMap::request(0);
      memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas=10;
      memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas=7;

              memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo=QString("%1").arg(TXT_ORIGEN_CONTADOR);
              memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion=ID_CONTADOR_CONFIGURACION_COLUMNAS;

              //offset_columna0;//=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.offset;
              dummy_index=(index-1);

              valor_inicial_fila=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila;//+incremento*(index-1);

                      for (unsigned int fila=0;fila<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas;fila++){
                          valor_actual=valor_inicial_fila;
                          for (unsigned int columna=0;columna<memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas;columna++){
                              for (unsigned int yy=0;yy<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].FilasMatrizCampo;yy++){
                                  for (unsigned int xx=0;xx<memoryMap->m_mapa.m_camposvariables[i].m_matrices[j].ColumnasMatrizCampo;xx++){
                                          //Si es un campo de tipo CONTADOR
                                          if (memoryMap->m_mapa.m_camposvariables[i].OrigenValorCampo==QString("%1").arg(TXT_ORIGEN_CONTADOR)){
                                              if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_COLUMNAS){
                                                  //Impresion por columnas
                                                  //=ValorInicial+Offset*(Columna-1)+Incremento*(Fila-1)+Filas*(Index-1)
                                                  //valor_actual=valor_inicial[i]+offset_columna*columna+incremento[i]*fila+memoryMap->m_mapa.m_variables.ImpresionesPorEnvioFilas*index;
                                                  //valor_actual=valor_inicial_fila;//memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual;
                                                  //if (valor_actual>limite_superior) valor_actual=limite_inferior;
                                                  //if (valor_actual<limite_inferior) valor_actual=limite_superior;
                                                  memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual=valor_actual;//valor_inicial_fila+incremento;
                                                  Vtemp=QString("%1").arg(valor_actual).rightJustified(10,'0',true);
                                                  //qDebug() << "valor_actual contador: " << valor_actual;
                                                  valor_actual+=offset_columna;
                                              }
                                              else if (memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.indexConfiguracion==ID_CONTADOR_CONFIGURACION_FILAS){
                                                  //Impresión por filas
                                                  //=ValorInicial+Offset*(Fila-1)+Incremento*(Columna-1)+Columnas*(Index-1)
                                                  //valor_actual=valor_inicial[i]+memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas*fila+incremento[i]*columna+memoryMap->m_mapa.m_variables.ImpresionesPorEnvioColumnas*index;
                                                  valor_actual=memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual;
                                                  if (valor_actual>limite_superior) valor_actual=limite_inferior;
                                                  if (valor_actual<limite_inferior) valor_actual=limite_superior;
                                                  memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorActual=valor_actual;
                                                  Vtemp=QString("%1").arg(valor_actual).rightJustified(10,'0',true);
                                                  qDebug() << "valor_actual barcode: " << valor_actual;
                                                  valor_actual+=incremento;
                                              }

                                              dummy_index++;

                                          }//si es un campo variablede tipo contador

                                  } // for xx

                              } // for yy
                          } //columna
                          valor_inicial_fila+=incremento;
                          memoryMap->m_mapa.m_camposvariables[i].m_counterParameters.valorInicialFila=valor_inicial_fila;
                      }//fila



  }

/** calcula el tiempo de parada*/
 int getTiempoParada(){
    int Distance1=0;
    MemoryMap *memoryMap=MemoryMap::request(0);
    int timeParada=TIMEOUT_PRINT_COMPLETE_LONG;
    if (memoryMap->m_mapa.m_maquina.velocidad>400){
        Distance1=((400+(memoryMap->m_mapa.m_maquina.velocidad-450)/2)/60)*30; //Como mucho estoy 2 sg decelerando, calculo su velocidad media y fuera
       timeParada=((memoryMap->m_mapa.m_machinedefaults.DistanciaStop-Distance1)*6000)/450+3000;
    }
    else timeParada=memoryMap->m_mapa.m_machinedefaults.DistanciaStop*60/450;
    if (timeParada<TIMEOUT_PRINT_COMPLETE) timeParada=TIMEOUT_PRINT_COMPLETE;
    return timeParada;
 }


 int generarNumeroAleatorio(int limiteInferior,int limiteSuperior){

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

 void cambiarSemillaNumeroAleatorio(void){
     int semilla=QTime::currentTime().toString("zzz").toInt();
     qsrand(semilla);
 }
