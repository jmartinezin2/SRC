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
#include "MemoryMap.h"
#include "globals.h"
#include <QApplication>
#include "in2defines.h"
#include <QPainter>
#include <QFile>
#include <QDateTime>
#include <QDate>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <qmath.h>

/*int mallocleft = 0;
int windowsleft = 0;*/
#define DEFAULT_WAIT_TIME 5000

//Una sola instancia
MemoryMap *MemoryMap::mInstance  = NULL;


MemoryMap::MemoryMap(QObject *parent)
{

}

/** Lllamda a instancia unica*/
MemoryMap *MemoryMap::request(QObject *parent)
{
    if(mInstance == NULL){
        mInstance = new MemoryMap(parent);
    }
    return mInstance;
}



MemoryMap::~MemoryMap(void)
{
    //qDebug() << "Destruccion de memorymap";
}
/** Devuelve el user*/
QString MemoryMap::getUser(){
    return this->m_log.m_user;
}
/** Devuelve el cliente*/
QString MemoryMap::getClient(){
    return this->m_log.m_cliente;
}

/** Devuelve el password*/
QString MemoryMap::getPassword(){
    return this->m_log.m_Password;
}
/** Carga user y Password*/
void MemoryMap::setUserAndPassword(const QString &user, const QString &password){
    this->m_log.m_user=user;
    this->m_log.m_Password=password;
}
/** Carga user y Password*/
void MemoryMap::setUser(const QString &user){
    this->m_log.m_user=user;
}
/** Carga cliente*/
void MemoryMap::setClient(const QString &client){
    this->m_log.m_cliente=client;
}

/** Carga derechos de usuario*/
void MemoryMap::setUserRights(int derechos){
    m_log.m_derechos=derechos;
}
/** Devuelve los derechos de usuario*/
int MemoryMap::getUserRights(){
    return m_log.m_derechos;
}
/** Devuelve un texto con los permisos*/
QString MemoryMap::getUserRightsText(){
    QString strRight="NONE";
    int right=getUserRights();
    if (right==ACCESS_PRINTER) strRight="PRINTER";
    else if (right==ACCESS_ROOT) strRight="ADMIN";
    else if (right==ACCESS_MAINTENANCE) strRight="MANTENIMIENTO";
    else if (right==ACCESS_GOD) strRight="SUPERUSER";
    return strRight;
}

/** Testea la politica password*/
int MemoryMap::TestPasswordPolicy(){
    int rtn=PASSWORD_POLICY_OK;
    if (!m_log.m_Password.isEmpty()){
        if (m_log.m_Password.length()<PASSWORD_POLICY_LEN) rtn=PASSWORD_POLICY_LEN;
    }
    return rtn;
}


//Inicializa el mapa de memoria
void MemoryMap::Init(){
    this->ImagenVariable=0;

    m_mapa.m_machinedefaults.WebWidth=DEFAULT_WEB_WIDTH;
    m_mapa.m_machinedefaults.WebHeight=DEFAULT_WEB_HEIGHT;
    m_mapa.m_machinedefaults.MaculaTrasPrint=true;
    m_mapa.m_machinedefaults.DistanciaStop=DEFAULT_DISTANCIA_STOP;
    m_mapa.m_machinedefaults.VelArranque=VEL_MIN_ARRANQUE;
    m_mapa.m_machinedefaults.TiempoArranque=TIEMPO_ARRANQUE;
    m_mapa.m_machinedefaults.JointTime=TIEMPO_ARRANQUE_EMPALME;
    m_mapa.m_machinedefaults.ControlHead=DEFAULT_CONTROL_HEAD;
    m_mapa.m_machinedefaults.filtroMaculas=false;
    m_mapa.m_machinedefaults.delayedMonitorTime=START_TIMED;
    m_mapa.m_machinedefaults.DiametroMaximoBobinaEntrada=DIAMETRO_BOBINA_ENTRADA_MAX;
    m_mapa.m_machinedefaults.DiametroMinimoBobinaEntrada=DIAMETRO_BOBINA_ENTRADA_MIN;
    m_mapa.m_machinedefaults.DiametroMaximoBobinaSalida=DIAMETRO_BOBINA_ENTRADA_MAX;
    m_mapa.m_machinedefaults.DiametroMinimoBobinaSalida=DIAMETRO_BOBINA_ENTRADA_MIN;
    m_mapa.m_machinedefaults.DiametroMinimoEmpalme=DIAM_LIMIT_EMPALME;
    m_mapa.m_machinedefaults.DiametroMinimoBultoEntrada=DIAMETRO_MINIMO_BULTO_ENTRADA;
    m_mapa.m_machinedefaults.DiametroMinimoBultoSalida=DIAMETRO_MINIMO_BULTO_SALIDA;

    m_log.m_user="In2Daemon";
    m_log.m_cliente=CLIENT_IN2;
    m_log.m_Password.clear();
    m_log.m_derechos=0;
    //Limpiamos el mapa
    m_DebugLine.clear();
    m_mapa.m_config.MotorImpresion.clear();
    m_mapa.m_config.MotorFluidica.clear();
    m_mapa.m_config.OPCEnable=false;
    m_mapa.m_config.OPCLibreria.clear();
    m_mapa.m_config.NumColores=1;
    m_mapa.m_config.BitDepth=0;
    m_mapa.m_config.TipoColor.clear();
    m_mapa.m_config.TecladoVirtualEnable=false;
    m_mapa.m_config.RegistroUsuario=0;
    m_mapa.m_config.m_LDAPActive=false;

    //memset(&m_mapa.m_database,0x00,sizeof(m_mapa.m_database));
    m_mapa.m_database.NombreDb.clear();
    m_mapa.m_database.RutaDb.clear();
    m_mapa.m_database.NombreRutaDb.clear();
    m_mapa.m_database.NombreTablaDatosImprimir.clear();
    m_mapa.m_database.NombreColaImpresion.clear();
    m_mapa.m_database.RutaColaImpresion.clear();
    m_mapa.m_database.FilasTotalesDB=0;
    m_mapa.m_database.ImpresionesEnUltimoEnvio=0;
    m_mapa.m_database.ImpresionesPorEnvio=0;
    m_mapa.m_database.ResultadosTotalesDB=0;
    m_mapa.m_database.NumCamposTotalesDB=0;
    m_mapa.m_database.EnviosNecesariosDB=0;
    m_mapa.m_database.PermitirRepeticiones=0;
    m_mapa.m_variables.bUseLocalCopy=false;
    m_mapa.m_variables.bReloadCompo=true;
    m_mapa.m_variables.SalvarFicheroConfiguracion=0;
    m_mapa.m_variables.DisablePrinting=0;
    m_mapa.m_variables.ImpresionesPorEnvioFilas=1;
    m_mapa.m_variables.ImpresionesPorEnvioColumnas=1;
    m_mapa.m_variables.DefaultNumHeads=8;
    m_mapa.m_sai.testSai=false;
    m_mapa.m_sai.tokenFile.clear();
    //memset(&m_mapa.m_variables.VolumenGota,0x00,sizeof(m_mapa.m_variables.VolumenGota));

    m_mapa.m_variables.defaultImagePath=QApplication::applicationDirPath(); //Por defecto a la ruta de aplicacion
    InitComposicion();
    m_mapa.m_impresion.bEscalaGrises=false;
    m_mapa.m_impresion.bHighSpeed=false;
    m_mapa.m_impresion.bReverse=false;
    m_mapa.m_impresion.bMonitorHead=true;
    m_mapa.m_impresion.m_fltrMacula.bMonitorMacula=false;
    m_mapa.m_impresion.m_fltrMacula.fDistancia=0;
    m_mapa.m_impresion.m_fltrMacula.fError=0;
    m_mapa.m_impresion.m_fltrMacula.fMax=0;
    m_mapa.m_impresion.m_fltrMacula.fMin=0;
    m_mapa.m_impresion.m_fltrMacula.fMaxFiltered=0;
    m_mapa.m_impresion.m_fltrMacula.fMinFiltered=0;
    m_mapa.m_impresion.iVelocidad=DEFAULT_SPEED;
    m_mapa.m_impresion.qstrPal1=DEFAULT_XPALETTE;
    m_mapa.m_impresion.qstrPal2=DEFAULT_XPALETTE;
    m_mapa.m_impresion.qstrPal3=DEFAULT_XPALETTE;
    m_mapa.m_impresion.qstrPal4=DEFAULT_XPALETTE;
    m_mapa.m_maquina.VersionMaquina=0;
    m_mapa.m_maquina.qmDiamPresion.clear();
    m_mapa.m_maquina.softstopear=false;
    m_mapa.m_maquina.stopear=false;
    m_mapa.m_maquina.impresion=false;
    m_mapa.m_maquina.MaquinaEnAutomatico=0;
    m_mapa.m_maquina.InicioSecuenciaEmpalme=0;
    m_mapa.m_maquina.InicioSecuenciaAutomatico=0;
    m_mapa.m_maquina.InicioSecuenciaBobinaNueva=0;
    m_mapa.m_maquina.EstadoSecuenciaBobinaNueva=0;
    m_mapa.m_maquina.EmpalmePerformed=false;
    m_mapa.m_maquina.ParadaDiametro=false;
    m_mapa.m_maquina.prev_ref_velocidad=0;
    m_mapa.m_maquina.ref_velocidad=0;
    m_mapa.m_maquina.ref_velocidad_actual=0;
    m_mapa.m_maquina.velocidad=0;
    m_mapa.m_maquina.seg_lampara_on=0;
    m_mapa.m_maquina.contador_velocidad=0;
    m_mapa.m_maquina.metros_Offset=0;
    m_mapa.m_maquina.modoParo=0;
    m_mapa.m_maquina.ref_metros_trabajo=0;
    m_mapa.m_maquina.metros_trabajo=0;
    m_mapa.m_maquina.iCounter=0;
    m_mapa.m_maquina.iRefCounter=0;
    m_mapa.m_maquina.iTCounter=0;
    m_mapa.m_maquina.iOffsetCounter=0;
    m_mapa.m_maquina.ref_diametro_bobina_entrada=0;
    m_mapa.m_maquina.diametro_bobina_entrada=0;
    m_mapa.m_maquina.diametro_bobina_entrada_inst=0;
    m_mapa.m_maquina.iContadorParada=0;
    m_mapa.m_maquina.ref_diametro_bobina_salida=0;
    m_mapa.m_maquina.diametro_bobina_salida=0;
    m_mapa.m_maquina.diametro_bobina_salida_inst=0;
    m_mapa.m_maquina.diametro_parar=0;
    m_mapa.m_maquina.iMaxMacula=0;
    m_mapa.m_maquina.iMinMacula=0;
    m_mapa.m_maquina.iContadorMaculas=0;
    m_mapa.m_maquina.secuencia_on=false;
    m_mapa.m_maquina.condiciones_impresion=false;
    m_mapa.m_maquina.condiciones_movimiento=false;
    m_mapa.m_maquina.codigo_parada=0;
    m_mapa.m_maquina.softstop_realizado=false;
    m_mapa.m_maquina.stop_realizado=false;
    m_mapa.m_maquina.jogmas_conlampara=false;
    m_mapa.m_maquina.jogmenos_conlampara=false;
    m_mapa.m_maquina.HorasMaximasUsoLampara=0;
    m_mapa.m_maquina.TemperaturaMaximaLampara=0;
    m_mapa.m_maquina.DiasMaximosLimpiezaLampara=0;
    m_mapa.m_maquina.DiasMaximosUsoFiltro=0;
    m_mapa.m_maquina.AnchoBobina=0;
    m_mapa.m_maquina.temperatura_lampara=0;
    m_mapa.m_maquina.ContadorTempLampara=0;
    m_mapa.m_maquina.intensidad_lampara=0;
#ifdef BALLUF
    m_mapa.m_maquina.balluf_centrado=false;
#else
   m_mapa.m_maquina.balluf_centrado=true;
#endif

    for (DWORD i=0;i<MAXSCORPIONS;i++){
        m_mapa.m_maquina.m_xusbs[i].estado=0;
        m_mapa.m_maquina.m_xusbs[i].SN.clear();
        m_mapa.m_variables.VolumenGota[i]="00000000";
    }
    m_mapa.m_maquina.m_opc.estado=0;
    m_mapa.m_maquina.m_opc.Nombre.clear();

    m_mapa.m_maquina.m_fluidica.Conectado=0;
    m_mapa.m_maquina.m_fluidica.Estado=0;
    m_mapa.m_maquina.m_fluidica.Tipo=0;
    m_mapa.m_maquina.m_fluidica.NumEquipos=0;
    m_mapa.m_maquina.m_fluidica.NumConfigured=0;
    m_mapa.m_maquina.m_fluidica.Descripcion.clear();
    m_mapa.m_maquina.m_fluidica.error=0;
    for (int i=0;i<MAXHYDRAS;i++){
        m_mapa.m_maquina.m_fluidica.errormode[i]=0;
        m_mapa.m_maquina.m_fluidica.Used[i]=false;
        m_mapa.m_maquina.m_fluidica.bAutostart[i]=false;
        m_mapa.m_maquina.m_fluidica.bAutoShutdown[i]=false;
    }
    memset(&m_mapa.m_maquina.m_fluidica.ID,0x00,sizeof(m_mapa.m_maquina.m_fluidica.ID));
    memset(&m_mapa.m_maquina.m_fluidica.Modo,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Modo));
    memset(&m_mapa.m_maquina.m_fluidica.Supply_Temp,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Supply_Temp));
    memset(&m_mapa.m_maquina.m_fluidica.Level_Res,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Level_Res));
    memset(&m_mapa.m_maquina.m_fluidica.Supply_Pr,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Supply_Pr));
    memset(&m_mapa.m_maquina.m_fluidica.Vaccuum_Pr,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Vaccuum_Pr));
    memset(&m_mapa.m_maquina.m_fluidica.Pressure_Pr,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Pressure_Pr));
    memset(&m_mapa.m_maquina.m_fluidica.Return_Temp,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Return_Temp));
    memset(&m_mapa.m_maquina.m_fluidica.Res_Temp,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Res_Temp));
    memset(&m_mapa.m_maquina.m_fluidica.Buffer_Nivel,0xFF,sizeof(m_mapa.m_maquina.m_fluidica.Buffer_Nivel));
    memset(&m_mapa.m_maquina.m_fluidica.Nivel_filtrado,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Nivel_filtrado));
    memset(&m_mapa.m_maquina.m_fluidica.Nivel_filtrado_f,0x00,sizeof(m_mapa.m_maquina.m_fluidica.Nivel_filtrado_f));

    m_mapa.m_Vision.cErrores=0;
    m_mapa.m_Vision.cErroresConsecutivos=0;
    m_mapa.m_Vision.iPrograma=0;

    m_mapa.m_Marca.MarcaSupX=0;
    m_mapa.m_Marca.MarcaSupY=0;
    m_mapa.m_Marca.MarcaInfX=0;
    m_mapa.m_Marca.MarcaInfY=0;
    m_mapa.m_Marca.MarcaSupOnOff=0;
    m_mapa.m_Marca.MarcaInfOnOff=0;
    m_mapa.m_Marca.MarcaSupRepeticiones=0;
    m_mapa.m_Marca.MarcaSupFrecuencia=1;
    m_mapa.m_Marca.MarcaInfRepeticiones=0;
    m_mapa.m_Marca.MarcaInfFrecuencia=1;
    m_mapa.m_Marca.MarcaSupColor=0;
    m_mapa.m_Marca.MarcaInfColor=0;
    m_mapa.m_Marca.MarcaSupTipo=0;
    m_mapa.m_Marca.MarcaInfTipo=0;
    m_mapa.m_Marca.MarcaSupValor.clear();
    m_mapa.m_Marca.MarcaInfValor.clear();
    m_mapa.m_Marca.MarcaSupColor=1;
    m_mapa.m_Marca.MarcaSupFrecuencia=0;
    m_mapa.m_Marca.MarcaSupRepeticiones=1;
    m_mapa.m_Marca.MarcaInfColor=1;
    m_mapa.m_Marca.MarcaInfFrecuencia=1;
    m_mapa.m_Marca.MarcaInfRepeticiones=1;
    m_mapa.m_Marca.anguloSup=0;
    m_mapa.m_Marca.anguloInf=0;

    m_mapa.m_Marca.barcodeSup.BgColor=QColor(Qt::white);
    m_mapa.m_Marca.barcodeSup.borderType=0;
    m_mapa.m_Marca.barcodeSup.borderWidth=1;
    m_mapa.m_Marca.barcodeSup.CampoFijo=1;
    m_mapa.m_Marca.barcodeSup.codificacion=20;
    m_mapa.m_Marca.barcodeSup.escala=1;
    m_mapa.m_Marca.barcodeSup.FgColor=QColor(Qt::black);
    m_mapa.m_Marca.barcodeSup.fontSizeLarge=12;
    m_mapa.m_Marca.barcodeSup.fontSizeSmall=10;
    m_mapa.m_Marca.barcodeSup.fuente=QString("Arial");
    m_mapa.m_Marca.barcodeSup.height_mm=5.0;
    m_mapa.m_Marca.barcodeSup.height=50;
    m_mapa.m_Marca.barcodeSup.hideText=true;
    m_mapa.m_Marca.barcodeSup.indiceBgColor=0;
    m_mapa.m_Marca.barcodeSup.indiceFgColor=0;
    m_mapa.m_Marca.barcodeSup.inputMode=0;
    m_mapa.m_Marca.barcodeSup.lastError.clear();
    m_mapa.m_Marca.barcodeSup.NumeroCampo=0;
    m_mapa.m_Marca.barcodeSup.option2=0;
    m_mapa.m_Marca.barcodeSup.texto.clear();
    m_mapa.m_Marca.barcodeSup.whitespace=0;
    m_mapa.m_Marca.barcodeSup.width_mm=25.0;
    m_mapa.m_Marca.barcodeSup.width=250;
    m_mapa.m_Marca.barcodeSup.ar=Zint::QZint::IgnoreAspectRatio;
    m_mapa.m_Marca.barcodeSup.nivelSeguridad=0;
    m_mapa.m_Marca.barcodeSup.textAlignement=Qt::AlignHCenter;


    m_mapa.m_Marca.barcodeInf.BgColor=QColor(Qt::white);
    m_mapa.m_Marca.barcodeInf.borderType=0;
    m_mapa.m_Marca.barcodeInf.borderWidth=1;
    m_mapa.m_Marca.barcodeInf.CampoFijo=1;
    m_mapa.m_Marca.barcodeInf.codificacion=20;
    m_mapa.m_Marca.barcodeInf.escala=1;
    m_mapa.m_Marca.barcodeInf.FgColor=QColor(Qt::black);
    m_mapa.m_Marca.barcodeInf.fontSizeLarge=12;
    m_mapa.m_Marca.barcodeInf.fontSizeSmall=10;
    m_mapa.m_Marca.barcodeInf.fuente=QString("Arial");
    m_mapa.m_Marca.barcodeInf.height_mm=5.0;
    m_mapa.m_Marca.barcodeInf.height=50;
    m_mapa.m_Marca.barcodeInf.hideText=true;
    m_mapa.m_Marca.barcodeInf.indiceBgColor=0;
    m_mapa.m_Marca.barcodeInf.indiceFgColor=0;
    m_mapa.m_Marca.barcodeInf.inputMode=0;
    m_mapa.m_Marca.barcodeInf.lastError.clear();
    m_mapa.m_Marca.barcodeInf.NumeroCampo=0;
    m_mapa.m_Marca.barcodeInf.option2=0;
    m_mapa.m_Marca.barcodeInf.texto.clear();
    m_mapa.m_Marca.barcodeInf.whitespace=0;
    m_mapa.m_Marca.barcodeInf.width_mm=25.0;
    m_mapa.m_Marca.barcodeInf.width=250;
    m_mapa.m_Marca.barcodeInf.ar=Zint::QZint::IgnoreAspectRatio;
    m_mapa.m_Marca.barcodeInf.nivelSeguridad=0;
    m_mapa.m_Marca.barcodeInf.textAlignement=Qt::AlignHCenter;


    m_Imagen.load(":/res/images/In2HR.png");
	m_ConfiguracionCargada=false;
	m_ComposicionGenerada=false;
    m_ImagenEnviada=false;
    m_UltimaAccion=UA_NONE;
    /*QFile File;
	File.setFileName("./config/Config.bin");
	QString filename_configbin;
    filename_configbin=File.fileName();*/
    m_FilePath.clear();

}
/** Devuelve el numero de campos variables del mapa*/
int MemoryMap::getNumCamposVariables(){
    int numCampos=0;
    for (int i=0;i<NUM_CAMPOS_VARIABLES;i++){
        if (m_mapa.m_camposvariables[i].OnOffCampo) numCampos++;
    }
    return numCampos;
}
/** Devuelve el numero de matrices por campo variable*/
int MemoryMap::getNumMatricesCampoVariable(int index){
    int numMatrices=0;
    for (int i=0;i<NUM_MATRIZ_POR_CAMPO;i++){
        if (m_mapa.m_camposvariables[index].m_matrices[i].OnOffMatriz) numMatrices++;
    }
    return numMatrices;
}

//Inicializa valores por defecto de una composicion
void MemoryMap::InitComposicion(){


    //Datos del ini
    m_mapa.m_variables.NombreIni.clear();
    m_mapa.m_variables.RutaIni.clear();
    m_mapa.m_variables.NombreRutaIni.clear();

    m_mapa.m_variables.savedNombreIni.clear();
    m_mapa.m_variables.savedRutaIni.clear();
    m_mapa.m_variables.savedNombreRutaIni.clear();

    //Datos del pdf
    m_mapa.m_variables.NombrePdf.clear();
    m_mapa.m_variables.RutaPdf.clear();
    m_mapa.m_variables.NombreRutaPdf.clear();
    //Datos del bmp generado
    m_mapa.m_variables.NombreBmp.clear();
    m_mapa.m_variables.RutaBmp.clear();
    m_mapa.m_variables.NombreRutaBmp.clear();
    //Datos del bmp enviado desde Trabajos Impresion
    m_mapa.m_variables.NombreBmpEnviado.clear();
    m_mapa.m_variables.RutaBmpEnviado.clear();
    m_mapa.m_variables.NombreRutaBmpEnviado.clear();
    m_mapa.m_variables.NombreArchivoPost.clear();
    m_mapa.m_database.NombreDb.clear();
    m_mapa.m_database.NombreRutaDb.clear();
    m_mapa.m_database.NombreRutaDbIntermedia.clear();
    m_mapa.m_database.RutaDb.clear();

    m_mapa.m_variables.version_campos=0;
    m_mapa.m_variables.CreadoPor.clear();
    m_mapa.m_variables.CreadoCuando.clear();
    m_mapa.m_variables.ModificadoPor.clear();
    m_mapa.m_variables.ModificadoCuando.clear();
    m_mapa.m_variables.WebWidth=DEFAULT_WEB_WIDTH;
    m_mapa.m_variables.WebHeight=DEFAULT_WEB_HEIGHT;
    m_mapa.m_variables.Comentario.clear();
    m_mapa.m_variables.AnguloPlantilla=0;
    m_mapa.m_variables.AnguloCorte=0;
    m_mapa.m_variables.ImpresionesPorEnvio=0;
    m_mapa.m_variables.FilasPlantilla=0;
    m_mapa.m_variables.ColumnasPlantilla=0;
    m_mapa.m_variables.XinicialPlantilla=0;
    m_mapa.m_variables.YinicialPlantilla=0;
    m_mapa.m_variables.AnchoPlantilla=DEFAULT_WEB_WIDTH;
    m_mapa.m_variables.AltoPlantilla=DEFAULT_WEB_HEIGHT;
    m_mapa.m_variables.AlgdobleRes=false;
    m_mapa.m_variables.iBitsDithering=1;
    m_mapa.m_variables.SeparacionX=0;
    m_mapa.m_variables.SeparacionY=0;
    m_mapa.m_variables.FactorEscala=0;
    m_mapa.m_variables.PuntoInicialX=0;
    m_mapa.m_variables.PuntoInicialY=0;
    m_mapa.m_variables.MaculaIzqdaX=0;
    m_mapa.m_variables.MaculaIzqdaY=0;
    m_mapa.m_variables.MaculaIzqdaAncho=0;
    m_mapa.m_variables.MaculaIzqdaAlto=0;
    m_mapa.m_variables.MaculaDchaX=0;
    m_mapa.m_variables.MaculaDchaY=0;
    m_mapa.m_variables.MaculaDchaAncho=0;
    m_mapa.m_variables.MaculaDchaAlto=0;
    m_mapa.m_variables.MaculaIzqdaOnOff=0;
    m_mapa.m_variables.MaculaDchaOnOff=0;
    m_mapa.m_variables.MaculaDchaRepeticiones=0;
    m_mapa.m_variables.MaculaDchaFrecuencia=1;
    m_mapa.m_variables.MaculaIzqdaRepeticiones=0;
    m_mapa.m_variables.MaculaIzqdaFrecuencia=1;
    m_mapa.m_variables.MaculaIzqdaColor=0;
    m_mapa.m_variables.MaculaDchaColor=0;


    m_mapa.m_variables.NumColores=1;
    m_mapa.m_variables.AlgSelColor=SELECCION_MANUAL;
    m_mapa.m_variables.iGrayTh=10;
    m_mapa.m_variables.TipoColor.clear(); //Si es cuatricomía o colores plenos
    m_mapa.m_variables.OrdenFabricacion.clear();
    m_mapa.m_variables.Proceso=OFFLINE_PROC;
    m_mapa.m_variables.Producto.clear();
    m_mapa.m_variables.bDilateAlgorithm=false;
    m_mapa.m_variables.iSabreAlgorithm=0;
    //Esta es la zona de plantilla
    m_mapa.m_variables.bPlantEnabled=false;
    m_mapa.m_variables.fPlantAncho=DEFAULT_WEB_WIDTH;
    m_mapa.m_variables.fPlantAlto=DEFAULT_WEB_HEIGHT;
    m_mapa.m_variables.fPlantOffsetX=0;
    m_mapa.m_variables.fPlantOffsetY=0;
    m_mapa.m_variables.fPlantDistCols=0;
    m_mapa.m_variables.iPlantCols=1;
    m_mapa.m_variables.strPlantImages.clear();



    m_mapa.m_variables.MaculaIzqdaColor=1;
    m_mapa.m_variables.MaculaIzqdaAlto=10;
    m_mapa.m_variables.MaculaIzqdaAncho=10;
    m_mapa.m_variables.MaculaIzqdaFrecuencia=1;
    m_mapa.m_variables.MaculaIzqdaRepeticiones=1;

    m_mapa.m_variables.MaculaDchaColor=1;
    m_mapa.m_variables.MaculaDchaAlto=10;
    m_mapa.m_variables.MaculaDchaAncho=10;
    m_mapa.m_variables.MaculaDchaFrecuencia=1;
    m_mapa.m_variables.MaculaDchaRepeticiones=1;

    m_mapa.m_variables.WebWidth=DEFAULT_WEB_WIDTH;
    m_mapa.m_variables.WebHeight=DEFAULT_WEB_HEIGHT;
    m_mapa.m_variables.ImpresionesPorEnvioColumnas=1;
    m_mapa.m_variables.ImpresionesPorEnvioFilas=1;
    m_mapa.m_variables.ImpresionesPorEnvio=1;
    m_mapa.m_variables.algoritmoSuperposicion=true;

    //Color de máscara
    m_mapa.m_ColorMascara.valor=QColor(Qt::black).rgb();
    m_mapa.m_ColorMascara.x_pix=DEFAULT_NOMASK_PIXEL;
    m_mapa.m_ColorMascara.y_pix=DEFAULT_NOMASK_PIXEL;

    for (int x=0;x<NUM_COLORES;x++){
        m_mapa.m_colores[x].index=x;
        m_mapa.m_colores[x].valor=QColor(Qt::white).rgb();
        m_mapa.m_colores[x].x_pix=DEFAULT_NOMASK_PIXEL;
        m_mapa.m_colores[x].y_pix=DEFAULT_NOMASK_PIXEL;
        m_mapa.m_colores[x].cadena.clear();
        m_mapa.m_colores[x].enabled=false;
        m_mapa.m_colores[x].alg.clear();
        m_mapa.m_colores[x].bWhiteCorr=false;
        m_mapa.m_colores[x].bBlackCorr=false;
        m_mapa.m_colores[x].bMonoMode=false;
        m_mapa.m_colores[x].bMonoCorr=false;
        m_mapa.m_colores[x].bCorrDinamica=false;
        m_mapa.m_colores[x].m_diherCut=CUT_LINEAL;
        m_mapa.m_colores[x].m_ditherPal.clear();
        m_mapa.m_colores[x].m_customPal=false;


    }
    //memset(&m_mapa.m_campos,0x00,sizeof(m_mapa.m_campos));
    for (int x=0;x<NUM_CAMPOS;x++){
        m_mapa.m_campos[x].color=1;
        m_mapa.m_campos[x].EstiloLetraCampo=QString("Normal");
        m_mapa.m_campos[x].NombreCampo.clear();
        m_mapa.m_campos[x].ValorActualCampo=QString("Texto campo %1").arg(x);
        m_mapa.m_campos[x].NumCaracteresCampo=0;
        m_mapa.m_campos[x].OnOffCampo=0;
        m_mapa.m_campos[x].TamanoFuenteCampo=DEFAULT_FONT_SIZE;
        m_mapa.m_campos[x].letterSpace=0;
        m_mapa.m_campos[x].TipoCampo=TXT_CAMPO_TIPO_TEXTO;
        m_mapa.m_campos[x].TipoLetraCampo=QString("Arial");
        //memset(m_mapa.m_campos[x].m_matrices,0x00,sizeof(m_mapa.m_campos[x].m_matrices));
        for (int i=0;i<NUM_MATRIZ_POR_CAMPO;i++){
            m_mapa.m_campos[x].m_matrices[i].ColumnasMatrizCampo=1;
            m_mapa.m_campos[x].m_matrices[i].FilasMatrizCampo=1;
            m_mapa.m_campos[x].m_matrices[i].ExcepcionesMatrizCampo.clear();
            m_mapa.m_campos[x].m_matrices[i].FreqColumnasMatrizCampo=0;
            m_mapa.m_campos[x].m_matrices[i].FreqFilasMatrizCampo=0;
            m_mapa.m_campos[x].m_matrices[i].incFilas=0;
            m_mapa.m_campos[x].m_matrices[i].incColumnas=0;
            m_mapa.m_campos[x].m_matrices[i].OnOffMatriz=0;
            m_mapa.m_campos[x].m_matrices[i].OrientacionCampo=0;
            m_mapa.m_campos[x].m_matrices[i].PosicionXCampo=0;
            m_mapa.m_campos[x].m_matrices[i].PosicionYCampo=(x*NUM_MATRIZ_POR_CAMPO+i)*DEFAULT_FONT_SIZE+2;

        }
        m_mapa.m_campos[x].m_barcodeParameters.BgColor=QColor(Qt::white);
        m_mapa.m_campos[x].m_barcodeParameters.borderType=0;
        m_mapa.m_campos[x].m_barcodeParameters.borderWidth=1;
        m_mapa.m_campos[x].m_barcodeParameters.CampoFijo=1;
        m_mapa.m_campos[x].m_barcodeParameters.codificacion=20;
        m_mapa.m_campos[x].m_barcodeParameters.escala=1;
        m_mapa.m_campos[x].m_barcodeParameters.FgColor=QColor(Qt::black);
        m_mapa.m_campos[x].m_barcodeParameters.fontSizeLarge=12;
        m_mapa.m_campos[x].m_barcodeParameters.fontSizeSmall=10;
        m_mapa.m_campos[x].m_barcodeParameters.fuente=QString("Arial");
        m_mapa.m_campos[x].m_barcodeParameters.height_mm=5.0;
        m_mapa.m_campos[x].m_barcodeParameters.height=50;
        m_mapa.m_campos[x].m_barcodeParameters.hideText=true;
        m_mapa.m_campos[x].m_barcodeParameters.indiceBgColor=0;
        m_mapa.m_campos[x].m_barcodeParameters.indiceFgColor=0;
        m_mapa.m_campos[x].m_barcodeParameters.inputMode=0;
        m_mapa.m_campos[x].m_barcodeParameters.lastError.clear();
        m_mapa.m_campos[x].m_barcodeParameters.NumeroCampo=x;
        m_mapa.m_campos[x].m_barcodeParameters.option2=0;
        m_mapa.m_campos[x].m_barcodeParameters.texto.clear();
        m_mapa.m_campos[x].m_barcodeParameters.whitespace=0;
        m_mapa.m_campos[x].m_barcodeParameters.width_mm=25.0;
        m_mapa.m_campos[x].m_barcodeParameters.width=250;
        m_mapa.m_campos[x].m_barcodeParameters.ar=Zint::QZint::IgnoreAspectRatio;
        m_mapa.m_campos[x].m_barcodeParameters.nivelSeguridad=0;
        m_mapa.m_campos[x].m_barcodeParameters.textAlignement=Qt::AlignHCenter;
    }
    for (int x=0;x<NUM_CAMPOS_VARIABLES;x++){
        m_mapa.m_camposvariables[x].NombreCampo.clear();
        m_mapa.m_camposvariables[x].ValorActualCampo.clear();
        m_mapa.m_camposvariables[x].OnOffCampo=0;
        m_mapa.m_camposvariables[x].TipoCampo=TXT_CAMPO_TIPO_TEXTO;
        m_mapa.m_camposvariables[x].OrigenValorCampo=TXT_ORIGEN_CONTADOR;
        m_mapa.m_camposvariables[x].indiceOrigenCampo=ID_ORIGEN_CONTADOR;
        m_mapa.m_camposvariables[x].OrdenCampo=0;
        m_mapa.m_camposvariables[x].TamanoFuenteCampo=DEFAULT_FONT_SIZE;
        m_mapa.m_camposvariables[x].letterSpace=0;
        m_mapa.m_camposvariables[x].TipoLetraCampo=QString("Arial");
        m_mapa.m_camposvariables[x].EstiloLetraCampo=QString("Normal");
        m_mapa.m_camposvariables[x].color=1;
        m_mapa.m_camposvariables[x].IndiceCampoEnDB=0;
        /*m_mapa.m_camposvariables[x].m_counterParameters.limiteInferior=0;
        m_mapa.m_camposvariables[x].m_counterParameters.limiteSuperior=0;
        m_mapa.m_camposvariables[x].m_counterParameters.valorInicial=0;
        m_mapa.m_camposvariables[x].m_counterParameters.incremento=0;*/
        //memset(m_mapa.m_camposvariables[x].m_matrices,0x00,sizeof(m_mapa.m_camposvariables[x].m_matrices));
        for (int i=0;i<NUM_MATRIZ_POR_CAMPO;i++){
            m_mapa.m_camposvariables[x].m_matrices[i].ColumnasMatrizCampo=1;
            m_mapa.m_camposvariables[x].m_matrices[i].FilasMatrizCampo=1;
            m_mapa.m_camposvariables[x].m_matrices[i].ExcepcionesMatrizCampo.clear();
            m_mapa.m_camposvariables[x].m_matrices[i].FreqColumnasMatrizCampo=0;
            m_mapa.m_camposvariables[x].m_matrices[i].FreqFilasMatrizCampo=0;
            m_mapa.m_camposvariables[x].m_matrices[i].incFilas=0;
            m_mapa.m_camposvariables[x].m_matrices[i].incColumnas=0;
            m_mapa.m_camposvariables[x].m_matrices[i].OnOffMatriz=0;
            m_mapa.m_camposvariables[x].m_matrices[i].OrientacionCampo=0;
            m_mapa.m_camposvariables[x].m_matrices[i].PosicionXCampo=0;
            m_mapa.m_camposvariables[x].m_matrices[i].PosicionYCampo=(x*NUM_MATRIZ_POR_CAMPO+i)*DEFAULT_FONT_SIZE+2;;

        }
        m_mapa.m_camposvariables[x].m_barcodeParameters.BgColor=QColor(Qt::white);
        m_mapa.m_camposvariables[x].m_barcodeParameters.borderType=0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.borderWidth=1;
        m_mapa.m_camposvariables[x].m_barcodeParameters.CampoFijo=1;
        m_mapa.m_camposvariables[x].m_barcodeParameters.codificacion=20;
        m_mapa.m_camposvariables[x].m_barcodeParameters.escala=1;
        m_mapa.m_camposvariables[x].m_barcodeParameters.FgColor=QColor(Qt::black);
        m_mapa.m_camposvariables[x].m_barcodeParameters.fontSizeLarge=12;
        m_mapa.m_camposvariables[x].m_barcodeParameters.fontSizeSmall=10;
        m_mapa.m_camposvariables[x].m_barcodeParameters.fuente=QString("Arial");
        m_mapa.m_camposvariables[x].m_barcodeParameters.height_mm=5.0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.height=50;
        m_mapa.m_camposvariables[x].m_barcodeParameters.hideText=true;
        m_mapa.m_camposvariables[x].m_barcodeParameters.indiceBgColor=0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.indiceFgColor=0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.inputMode=0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.lastError.clear();
        m_mapa.m_camposvariables[x].m_barcodeParameters.NumeroCampo=x;
        m_mapa.m_camposvariables[x].m_barcodeParameters.option2=0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.texto.clear();
        m_mapa.m_camposvariables[x].m_barcodeParameters.whitespace=0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.width_mm=25.0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.width=250;
        m_mapa.m_camposvariables[x].m_barcodeParameters.ar=Zint::QZint::IgnoreAspectRatio;
        m_mapa.m_camposvariables[x].m_barcodeParameters.nivelSeguridad=0;
        m_mapa.m_camposvariables[x].m_barcodeParameters.textAlignement=Qt::AlignHCenter;

        m_mapa.m_camposvariables[x].m_counterParameters.configuracion=TXT_CONTADOR_CONFIGURACION_COLUMNAS;
        m_mapa.m_camposvariables[x].m_counterParameters.incremento=1;
        m_mapa.m_camposvariables[x].m_counterParameters.indexConfiguracion=ID_CONTADOR_CONFIGURACION_COLUMNAS;
        m_mapa.m_camposvariables[x].m_counterParameters.limiteInferior=0;
        m_mapa.m_camposvariables[x].m_counterParameters.limiteSuperior=1000;
        m_mapa.m_camposvariables[x].m_counterParameters.numeroCampo=x;
        m_mapa.m_camposvariables[x].m_counterParameters.offset=1000;
        m_mapa.m_camposvariables[x].m_counterParameters.valorActual=0;
        m_mapa.m_camposvariables[x].m_counterParameters.valorInicial=0;
        m_mapa.m_camposvariables[x].m_counterParameters.valorInicialFila=0;
        m_mapa.m_camposvariables[x].m_counterParameters.numeroCifras=10;

        m_mapa.m_camposvariables[x].m_counterParameters.m_format.numeroCifras=10;
        m_mapa.m_camposvariables[x].m_counterParameters.m_format.zeroPadding=true;
        m_mapa.m_camposvariables[x].m_counterParameters.m_format.bFormatEnabled=false;
        QLocale locale;
        m_mapa.m_camposvariables[x].m_counterParameters.m_format.FormatSeparador=locale.language();

        m_mapa.m_camposvariables[x].m_randomNumberParameters.limiteInferior=1;
        m_mapa.m_camposvariables[x].m_randomNumberParameters.limiteSuperior=1000;
        m_mapa.m_camposvariables[x].m_randomNumberParameters.numeroCifras=3;
        m_mapa.m_camposvariables[x].m_randomNumberParameters.semilla=1;
        m_mapa.m_camposvariables[x].m_randomNumberParameters.valorActual=1;

    }

}

//Carga la imagen del fichero que se le diga
bool MemoryMap::setImagen(QString file){
	bool x;
	x=m_Imagen.load(file);
	if (x) m_FilePath=file;
	return x;
}

//Guarda el valor de la variable que se le diga
void MemoryMap::SetVar(int x, QString Valor){	
	
}
	//Devuelve el valor de la variable que le digamos
QString MemoryMap::GetVar(int index){
    return QString();
}

//Guarda en un fichero el mapa de memoria actual
int MemoryMap::SaveMemoryMap(){
         //int x;
	 return 0;
}

//Carga el fichero de memoria
int MemoryMap::LoadMemoryMap(){

    return 0;
}
//Devuelve la orde de fabricacion y el producto
QString MemoryMap::getOrdenAndProducto(){
    QString rtn=QString("%1@%2").arg(m_mapa.m_variables.OrdenFabricacion).arg(m_mapa.m_variables.Producto);
    return rtn;
}
/** Indica si se usa copia local*/
bool MemoryMap::useLocalCopy(){
    return m_mapa.m_variables.bUseLocalCopy;
}

/**Salta a la zona tempral*/
void MemoryMap::JumpToTempWorld(){
    if (this->useLocalCopy()){
        QString tmpPath=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(TEMP_PATH);
            QDir tmpDir(tmpPath);
            if (tmpDir.exists()){
                //Borramos lo que hay en el
                QStringList fileList=tmpDir.entryList();
                if (fileList.count()>2){ //Lod dos primeros son . y ..
                    for (int i=2;i<fileList.count();i++){
                        QString tmpFilePath=QString("%1\\%2").arg(tmpPath).arg(fileList.at(i));
                        QFile tmpFile(tmpFilePath);
                        if (tmpFile.exists()){
                            tmpFile.remove();
                        }
                    }
                }
            }
            else{ //No existe, lo creamos
                tmpDir.mkpath(tmpPath);
            }
            //Copio el fichero al local y trabajo con el
            ReturnToTempWorld();
            QFile iniFile(this->m_mapa.m_variables.savedNombreRutaIni);
            iniFile.copy(this->m_mapa.m_variables.NombreRutaIni);
    }
}
/** Vuelve a la zona temporal...sin borrar ni copiar nada*/
void MemoryMap::ReturnToTempWorld(){
    if (this->useLocalCopy()){
        QString tmpPath=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(TEMP_PATH);
        this->m_mapa.m_variables.savedNombreIni=this->m_mapa.m_variables.NombreIni;
        this->m_mapa.m_variables.savedRutaIni=this->m_mapa.m_variables.RutaIni;
        this->m_mapa.m_variables.savedNombreRutaIni=this->m_mapa.m_variables.NombreRutaIni;
        this->m_mapa.m_variables.RutaIni=tmpPath;
        this->m_mapa.m_variables.NombreRutaIni=QString("%1\\%2").arg(this->m_mapa.m_variables.RutaIni).arg(this->m_mapa.m_variables.NombreIni);
    }
}

/**Salta a la zona original*/
void MemoryMap::JumpToOriginalWorld(){
    if (this->useLocalCopy()){
        QString tmpPath=QString("%1\\%2").arg(QApplication::applicationDirPath()).arg(TEMP_PATH);
        QDir tmpDir(tmpPath);
        if (tmpDir.exists()){
            //Copiamos lo que hay en el
            QStringList fileList=tmpDir.entryList();
            if (fileList.count()>2){ //Los dos primeros son . y ..
                for (int i=2;i<fileList.count();i++){
                    QString tmpFilePath=QString("%1\\%2").arg(tmpPath).arg(fileList.at(i));
                    QFile tmpFile(tmpFilePath);
                    if (tmpFile.exists()){
                        QString finalDestFile=QString("%1\\%2").arg(this->m_mapa.m_variables.savedRutaIni).arg(fileList.at(i));
                        QFile::remove(finalDestFile);
                        if (tmpFile.copy(finalDestFile)) emit SendDebugInfo(LOGGER_WARNING,tr("%1 SAVED").arg(finalDestFile));
                        else emit SendDebugInfo(LOGGER_WARNING,tr("FAILURE SAVING %1").arg(finalDestFile));
                    }
                    else emit SendDebugInfo(LOGGER_WARNING,tr("MemMap:JumpToOriginalWorld: No existe tmpFilePath %1").arg(tmpFilePath));
                }
            }
            else emit SendDebugInfo(LOGGER_WARNING,tr("MemMap:JumpToOriginalWorld: ERROR fileListCount<=2"));
        }
        else emit SendDebugInfo(LOGGER_WARNING,tr("MemMap:JumpToOriginalWorld: No existe tmpPath %1").arg(tmpPath));
        ReturnToOriginalWorld();
    }
}
/** Vuelve a la zona original...sin borrar ni copiar nada*/
void MemoryMap::ReturnToOriginalWorld(){
    if (this->useLocalCopy()){
        this->m_mapa.m_variables.NombreIni=this->m_mapa.m_variables.savedNombreIni;
        this->m_mapa.m_variables.RutaIni=this->m_mapa.m_variables.savedRutaIni;
        this->m_mapa.m_variables.NombreRutaIni=this->m_mapa.m_variables.savedNombreRutaIni;
        QFileInfo tmpInfo(this->m_FilePath);
        QString tmpFile=tmpInfo.fileName();
        if (!tmpFile.isEmpty())
            this->m_FilePath=QString("%1\\%2").arg(this->m_mapa.m_variables.RutaIni).arg(tmpFile);
    }
}

/** Devuelve el path para buuscar la imagen*/
QString MemoryMap::getImagePath(){
    return m_mapa.m_variables.defaultImagePath;
}

/** Guarda quien crea la compo*/
void MemoryMap::SetCredentialsCreated(){
    QString dateTime=QString("%1 %2").arg(QDate::currentDate().toString("dd/MM/yyyy")).arg(QTime::currentTime().toString("hh:mm:ss"));
    this->m_mapa.m_variables.CreadoPor=this->getUser();
    this->m_mapa.m_variables.CreadoCuando=dateTime;
    this->m_mapa.m_variables.ModificadoCuando=this->m_mapa.m_variables.CreadoCuando;
    this->m_mapa.m_variables.ModificadoPor=this->m_mapa.m_variables.CreadoPor;
}
/** Guarda quien modifica la compo*/
void MemoryMap::SetCredentialsModified(){
    QString dateTime=QString("%1 %2").arg(QDate::currentDate().toString("dd/MM/yyyy")).arg(QTime::currentTime().toString("hh:mm:ss"));
    this->m_mapa.m_variables.ModificadoCuando=dateTime;
    this->m_mapa.m_variables.ModificadoPor=this->getUser();
}

/** Establece modo offline*/
void MemoryMap::setOfflineMode(){
    m_mapa.m_variables.Proceso=OFFLINE_PROC;
}

/** Devuelve el indice del campo*/
int MemoryMap::getIndexFixedData(QString val){
    int index=-1;
    for (int x=0;x<NUM_CAMPOS;x++){
        if (!m_mapa.m_campos[x].NombreCampo.compare(val)){
            index=x;
            break;
        }
    }
    return index;
}

/** Devuelve el indice del campo fijo mas cercano*/
int MemoryMap::getClosestFixedField(float xPos, float yPos){
    double minDistance=-1;
    int rtn=-1;
    for (int x=0;x<NUM_CAMPOS;x++){
        if (m_mapa.m_campos[x].OnOffCampo){
            for (int y=0;y<NUM_MATRIZ_POR_CAMPO;y++){
                if (m_mapa.m_campos[x].m_matrices[y].OnOffMatriz){
                    unsigned int tFilas=m_mapa.m_campos[x].m_matrices[y].FilasMatrizCampo;
                    unsigned int tCols=m_mapa.m_campos[x].m_matrices[y].ColumnasMatrizCampo;
                    double fFilas=m_mapa.m_campos[x].m_matrices[y].FreqFilasMatrizCampo;
                    double fCols=m_mapa.m_campos[x].m_matrices[y].FreqColumnasMatrizCampo;
                    double currX=0;
                    double currY=0;
                    double x0=m_mapa.m_campos[x].m_matrices[y].PosicionXCampo;
                    double y0=m_mapa.m_campos[x].m_matrices[y].PosicionYCampo;
                    for (unsigned int i=0;i<tFilas;i++){
                        for (unsigned int j=0;j<tCols;j++){
                            //Si esta en angulo, la posicion cambia
                            if ((m_mapa.m_variables.AnguloPlantilla==90)||(m_mapa.m_variables.AnguloPlantilla==270)){
                                currY=x0+j*fCols;
                                currX=y0+i*fFilas;
                            }
                            else{
                                currX=x0+j*fCols;
                                currY=y0+i*fFilas;
                            }
                            double dX=qPow((xPos-currX),2);
                            double dY=qPow((yPos-currY),2);
                            double distance=qSqrt(dX+dY);
                            if ((distance<minDistance)||(rtn==-1)){
                                rtn=x;
                                minDistance=distance;
                            }
                        }
                    }

                }
            }

        }
    }

    return rtn;

}

