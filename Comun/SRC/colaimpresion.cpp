/** @addtogroup ColaImpresion
 *  @{*/
/** @file ColaImpresion.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions
 *
 *    @brief Pantalla de configuracion de usuarioss
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ConfUsers.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#include "colaimpresion.h"
#include "globals.h"
#include "teclado.h"
#include <QMessageBox> 
#include <QHeaderView>
#include <QScrollBar>
#include <QTextEdit>
#include <QFile>
#include <QSettings>
#include "GestorDB_SQLITE.h"

#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>
#include <QTableView>
#include <QTimer>
#include "in2counters.h"
#include "in2message.h"
#include "in2compofile.h"


QStringList NombreColumnasColaImpresion=(QStringList()<<"Composicion"<<"Path"<<"Fecha"<<"OrdenFabricacion");

static QWidget *p;

//Las columnas
#define COLUMNA_NOMBRE 0
#define COLUMNA_PATH 1
#define COLUMNA_FECHA 2
#define COLUMNA_OF 3

#define CONF_FILE "In2ColaImpresion.ini"

#define DEFAULT_ROW_HEIGHT 50

ColaTrabajo::ColaTrabajo(QWidget *parent, bool bLoad)
    : /*InkjetForm(parent)*/ui(new Ui::ColaImpresionUI)
{	
    /*ui->setupUi(this);
    GlobalSetScreenSize(this);
    m_memoryMap=MemoryMap::request(0);
    //m_gestorDB=NULL;
    m_prev=parent;
    QPalette palette = ui->m_Tabla->palette();
    palette.setColor(QPalette::Highlight,Qt::red);
    ui->m_Tabla->setPalette(palette);
    //QMessageBox::warning(this,"ERROR","No se pudo conectar con el servidor configurado. Revise configuración",QMessageBox::Ok);
    QString path=QString("%1\\Config\\%2").arg(QApplication::applicationDirPath()).arg(CONF_FILE);
    QFile fich(path);
    if (fich.exists()){
        QSettings FicheroIni(path,QSettings::IniFormat,0);
        FicheroIni.beginGroup(QString("CHECKSERVER"));
        m_ConnODBC.m_enabled=FicheroIni.value(QString("Enabled"),false).toBool();
        m_ConnODBC.m_password=FicheroIni.value(QString("Password"),"").toString();
        m_ConnODBC.m_user=FicheroIni.value(QString("User"),"").toString();
        m_ConnODBC.m_select=FicheroIni.value(QString("SqlSelect"),"").toString();
        m_ConnODBC.m_connODBC=FicheroIni.value(QString("ConnODBC"),"").toString();
        if (m_ConnODBC.m_enabled){
            m_gestorDB=new GestorDBODBC();
            bool ok=m_gestorDB->open("QODBC",m_ConnODBC.m_connODBC,m_ConnODBC.m_user,m_ConnODBC.m_password);
            if (!ok){
                QMessageBox::warning(this,tr("ERROR"),tr("No se pudo conectar con el servidor configurado. Revise configuración"),QMessageBox::Ok);
            }
        }
        FicheroIni.endGroup();
    }
    iFila=0;
    iColumna=0;

    p=parent;
    //this->setDataCommon();
    int NumeroColumnasColaImpresion=NombreColumnasColaImpresion.count();

    ui->m_Tabla->setWindowTitle("Gestión de cola de impresión");
    ui->m_Tabla->setColumnCount(NumeroColumnasColaImpresion);
    ui->m_Tabla->setShowGrid(true);

    ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnasColaImpresion);

    ui->m_Tabla->setColumnWidth(0,ui->groupBoxCentral->width()/4);
    ui->m_Tabla->setColumnWidth(1,ui->groupBoxCentral->width()/4);
    ui->m_Tabla->setColumnWidth(2,ui->groupBoxCentral->width()/8);
    ui->m_Tabla->setColumnWidth(3,ui->groupBoxCentral->width()/8);

    ui->m_Tabla->setRowHeight(0,DEFAULT_ROW_HEIGHT);

#if QT_VERSION>=0x050000
     ui->m_Tabla->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
     ui->m_InfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->m_Tabla->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
    ui->m_InfoTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

#endif


    connect(ui->m_BackButton,SIGNAL(clicked()),this,SLOT(Atras()));

    if (!bLoad) ui->m_TrabajoSeleccionar->setEnabled(false);
    else
        connect(ui->m_TrabajoSeleccionar,SIGNAL(clicked()),this,SLOT(TrabajoSeleccionar()));

    connect(ui->m_TrabajoEliminar,SIGNAL(clicked()),this,SLOT(TrabajoEliminar()));
    connect(ui->m_Tabla,SIGNAL(cellPressed(int,int)),this,SLOT(SeleccionCampo(int, int)));
    //connect(ui->m_Tabla,SIGNAL(cellClicked(int,int)),this,SLOT(SeleccionCampo(int, int)));
    connect(ui->m_leTrabajoScan,SIGNAL(textEdited(QString)),this,SLOT(sltScanSelection(QString)));
    connect(ui->m_leTrabajoScan,SIGNAL(textChanged(QString)),this,SLOT(sltSelectText(QString)));
    connect(ui->m_pbCancelaScan,SIGNAL(clicked()),this,SLOT(sltCancelaScan()));
    connect(this, SIGNAL(SendInformerInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveInformerInfo(unsigned char,QString)));

    UpdateInfo();

	iFila=0;
	iColumna=0;
	TestRoot();
    m_autoload=true;
    //Por si utilizamos un lector
    ui->m_leTrabajoScan->setFocus();
    m_ScanEnabled=false;*/
}
/** Cancela el scan*/
void ColaTrabajo::sltCancelaScan(){
    ui->m_leTrabajoScan->clear();
    ui->m_leTrabajoScanCopy->clear();
}

/** Swelecciona un texto de manera automatica*/
void ColaTrabajo::sltSelectText(QString txt){
    bool  rtn=false;
    int x=0;
    for (x=0;x<ui->m_Tabla->rowCount();x++){
        QTableWidgetItem *p=ui->m_Tabla->item(x,0);
        if (p){
            if (p->text().contains(txt,Qt::CaseSensitive)){
                rtn=true;
                break;
            }
        }
    }
    if (rtn){
        ui->m_Tabla->selectRow(x);
    }

}


/** Cambia el valor de autoload**/
void ColaTrabajo::setAutoload(bool val){
    m_autoload=val;
}


ColaTrabajo::~ColaTrabajo()
{
    /*if (m_gestorDB!=NULL){
        m_gestorDB->close();
        delete (m_gestorDB);
    }*/
}

/** Se hace una seleccion por scanner*/
void ColaTrabajo::sltScanSelection(QString p){
    if (!m_ScanEnabled){
        m_ScanEnabled=true;
        QTimer::singleShot(2000,this,SLOT(sltCheckScan()));
    }
}

/** Se hace una seleccion por scanner*/
void ColaTrabajo::sltCheckScan(){

    /*ui->m_leTrabajoScanCopy->setText(ui->m_leTrabajoScan->text());
    ui->m_leTrabajoScan->clear();
    QString reg=QString("%1_OF").arg(ui->m_leTrabajoScanCopy->text());
    bool rtn=false;

    int x=0;
    for (x=0;x<ui->m_Tabla->rowCount();x++){
        QTableWidgetItem *p=ui->m_Tabla->item(x,COLUMNA_NOMBRE);
        //listaNombres.append(miDataBaseColaImpresion.m_Tabla.at[x]);
        if (p->text().contains(reg,Qt::CaseInsensitive)){
            rtn=true;
            break;
        }
    }
    if (rtn==true){
        ui->m_Tabla->selectRow(x); //Se selecciona toda la fila
        if (m_memoryMap->getUserRights()<ACCESS_ROOT){
            ui->m_TrabajoSeleccionar->setEnabled(true);
            ui->m_TrabajoEliminar->setEnabled(true);
         }
        iFila=x;
        iColumna=0;
    }*/
    m_ScanEnabled=false;
}

/** Si no es root, bloqueo funcionalidad.*/
void ColaTrabajo::TestRoot(){
    if (m_memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_TrabajoSeleccionar->setEnabled(false);
        ui->m_TrabajoEliminar->setEnabled(false);
    }
}
void ColaTrabajo::ActualizaDataSlot(){
    /*DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();*/
}

void ColaTrabajo::Atras(){
    if (m_autoload) emit ActualizaMaquina();
    emit FileSelected();
    this->deleteLater();

}

//Selecciona el trabajo seleccionado
void ColaTrabajo::TrabajoSeleccionar(){

    QString nombreruta,nombre,ruta;
    QString nombrerutaini,nombreini,rutaini;
    bool bRet,bRet_ini,bRet_CX;

    if (ui->m_Tabla->rowCount()==0) return;
    if (ui->m_Tabla->currentRow()<0) return;

    int ret = QMessageBox::warning(this, tr("INFO"),tr("Desea cargar el trabajo seleccionado?"),
    QMessageBox::Ok | QMessageBox::Cancel);
    if (ret==QMessageBox::Ok){
        //QLabel *dummyLabel=ShowMensaje("Cargando");
        In2Message *msg=new In2Message(this,"Cargando...Por favor espere");
        connect(this,SIGNAL(KillIn2Msg()),msg,SLOT(sltDelete()));
        nombre=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0)->text();
        ruta=ui->m_Tabla->item(ui->m_Tabla->currentRow(),1)->text();
        nombreruta=QString("%1\\%2").arg(ruta).arg(nombre);

        //m_memoryMap->m_mapa.m_variables.NombreRutaBmp=nombreruta;

        m_memoryMap->m_mapa.m_variables.OrdenFabricacion=ui->m_Tabla->item(ui->m_Tabla->currentRow(),3)->text();
        m_memoryMap->m_mapa.m_variables.Proceso=m_memoryMap->m_mapa.m_variables.OrdenFabricacion;
        QFile fichero(QString("%1").arg(nombreruta));
        bRet=fichero.exists();

        if (bRet){
            m_memoryMap->m_FilePath=nombreruta;
            m_memoryMap->m_Imagen.load(nombreruta);

            //nombrerutaini=nombreruta;
            //nombrerutaini=nombrerutaini.replace("_C1_comp.bmp",".ini");

            nombrerutaini=DevuelveNombreRutaIni(nombreruta);

            m_memoryMap->m_mapa.m_variables.NombreRutaIni=nombrerutaini;

            QFile fichero_ini(QString("%1").arg(nombrerutaini));
            bRet_ini=fichero_ini.exists();
            if(bRet_ini){
                In2CompoFile ficheroIni;
                ficheroIni.AbrirFicheroIni();
                bRet_CX=true;
                for(int i=1;i<m_memoryMap->m_mapa.m_variables.NumColores;i++){
                    QFile fichero_CX;
                    QString nombreruta_CX;
                    bool dummy_CX;

                    nombreruta_CX=nombreruta;
                    nombreruta_CX.replace("_C1_comp.bmp",QString("_C%1_comp.bmp").arg(i+1));
                    dummy_CX=fichero_CX.exists(nombreruta_CX);
                    if (dummy_CX==false) bRet_CX=false;
                }
                if(bRet_CX==false){
                    //delete (dummyLabel);
                    QMessageBox::warning(0,tr("ERROR"),tr("No existe algún fichero de la composición"));
                    emit SendInformerInfo(INFORMER_DEBUG,tr("Error 1 al cargar archivo de cola: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));
                }
                else{
                    m_memoryMap->m_UltimaAccion=UA_TRABAJO_SELECCIONADO;
                    nombreini=DevuelveNombre(nombrerutaini);
                    rutaini=DevuelveRuta(nombrerutaini);
                    m_memoryMap->m_mapa.m_variables.NombreIni=nombreini;
                    m_memoryMap->m_mapa.m_variables.RutaIni=rutaini;
                    m_memoryMap->m_mapa.m_variables.NombreRutaBmp=m_memoryMap->m_mapa.m_variables.NombreRutaIni;
                    m_memoryMap->m_mapa.m_variables.NombreRutaBmp.replace(".ini",".bmp");
                    //delete (dummyLabel);
                    //QMessageBox::warning(0,"ATENCIÓN","Trabajo cargado correctamente",QMessageBox::Ok);
                    emit SendInformerInfo(INFORMER_DEBUG,tr("Archivo de cola cargado: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));
                    Atras();
                }
            }
            else{
                //delete (dummyLabel);
                QMessageBox::warning(0,"ERROR",tr("No existe el fichero\n%1").arg(nombrerutaini),QMessageBox::Ok);
                emit SendInformerInfo(INFORMER_DEBUG,tr("Error 2 al cargar archivo de cola: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));

            }
        }
        else{
            //delete (dummyLabel);
            QMessageBox::warning(0,"ERROR",tr("No existe el fichero\n%1").arg(nombreruta),QMessageBox::Ok);
            emit SendInformerInfo(INFORMER_DEBUG,tr("Error 3 al cargar archivo de cola: %1").arg(m_memoryMap->m_mapa.m_variables.NombreRutaIni));

        }
    }
    emit KillIn2Msg();
}


//Elimina la usuario seleccionado
void ColaTrabajo::TrabajoEliminar(){


    QString tmp;

    QTableWidgetItem *aux_comp;
    QTableWidgetItem *aux_fecha;
    QTableWidgetItem *aux_ruta;


    QString nombre_bmp;
    QString ruta_bmp;
    QString nombreruta_bmp;

    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());
    int ret = QMessageBox::warning(this, tr("INFO"),tr("Desea eliminar el trabajo seleccionado?"),
    QMessageBox::Ok | QMessageBox::Cancel);
    if (ret==QMessageBox::Ok){
        //aux_comp=new QTableWidgetItem();
        aux_comp=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
        nombre_bmp=aux_comp->text();

        //aux_ruta=new QTableWidgetItem();
        aux_ruta=ui->m_Tabla->item(ui->m_Tabla->currentRow(),1);
        ruta_bmp=aux_ruta->text();

        nombreruta_bmp=QString("%1\\%2").arg(ruta_bmp).arg(nombre_bmp);

        //aux_fecha=new QTableWidgetItem();
        aux_fecha=ui->m_Tabla->item(ui->m_Tabla->currentRow(),2);

        tmp=QString("delete from ColaImpresion where %1='%2' and %3='%4'")
                .arg(NombreColumnasColaImpresion.at(0))
                .arg(aux_comp->text())
                .arg(NombreColumnasColaImpresion.at(2))
                .arg(aux_fecha->text());

        GestorDBSQLITE DBColaImpresion;
        if (DBColaImpresion.open(m_memoryMap->m_mapa.m_database.RutaColaImpresion,"ConexionColaImpresion")!=true){
            QString tmp=tr("Error abriendo fichero de cola de impresión %1").arg(m_memoryMap->m_mapa.m_database.RutaColaImpresion);
            QMessageBox::warning(0,"ERROR",tmp);
            return;
        }
        if (DBColaImpresion.exec(tmp)!=true){
            QMessageBox::warning(0,tr("ERROR ColaImpresion (TrabajoEliminar)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
        }

        else{
            ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
            UpdateInfo();
            for(int i=0;i<NUM_COLORES;i++){
                QFile fichero_CX;
                QString nombreruta_CX;
                bool dummy_CX;

                nombreruta_CX=nombreruta_bmp;
                nombreruta_CX.replace("_C1_comp.bmp",QString("_C%1_comp.bmp").arg(i+1));
                dummy_CX=fichero_CX.exists(nombreruta_CX);
                if (dummy_CX==true) fichero_CX.remove(nombreruta_CX);
            }
        }
        DBColaImpresion.close();
    }

    ui->m_TrabajoSeleccionar->setEnabled(true);
    ui->m_TrabajoEliminar->setEnabled(true);

		
}


//Se selecciona un campo de la tabla
void ColaTrabajo::SeleccionCampo(int x, int y){
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow()); //Se selecciona toda la fila
    if (m_memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_TrabajoSeleccionar->setEnabled(true);
        ui->m_TrabajoEliminar->setEnabled(true);
     }
    iFila=x;
    iColumna=y;

    //Sacamos los campos y los mostramosc en la zona de scan
    QTableWidgetItem *p=ui->m_Tabla->item(x,COLUMNA_NOMBRE);
    QString str=p->text();
    ui->m_leTrabajoScan->setText(str);

    p=ui->m_Tabla->item(x,COLUMNA_PATH);
    str=p->text();
    //ui->m_leRutaScan->setText(str);

    In2Counters newRecord;
    QString OT=ui->m_Tabla->item(ui->m_Tabla->currentRow(),3)->text();
    QString query=QString("select * from Trabajos where OT='%1'").arg(OT);
    QStringList data=newRecord.getRecord(query);

    if (data.count()){
        updateHistoryTable(data,IN2RECORD_CAMPOS);
    }
    else ui->m_InfoTable->clearContents();


}

/** Actualiza la tabla de historico*/
void ColaTrabajo::updateHistoryTable(QStringList data,int campos){
    int numTrabajos=data.count()/campos; //Numero de columnas que tiene la info de trabajo
    ui->m_InfoTable->clearContents();
    ui->m_InfoTable->setRowCount(numTrabajos);
    for (int x=0;x<numTrabajos;x++){
        QTableWidgetItem *pInicio=new QTableWidgetItem(data.at(x*campos+1));//Columna de inicio
        ui->m_InfoTable->setItem(x,0,pInicio);
        QTableWidgetItem *pFin=new QTableWidgetItem(data.at(x*campos+2));//Columna de fin
        ui->m_InfoTable->setItem(x,1,pFin);
        QTableWidgetItem *pMetros=new QTableWidgetItem(data.at(x*campos+3));//Columna de metros
        ui->m_InfoTable->setItem(x,2,pMetros);
        QTableWidgetItem *pImpresiones=new QTableWidgetItem(data.at(x*campos+4));//Columna de impresiones
        ui->m_InfoTable->setItem(x,3,pImpresiones);
        QTableWidgetItem *pComentario=new QTableWidgetItem(data.at(x*campos+5));//Columna de comentarios
        ui->m_InfoTable->setItem(x,4,pComentario);
    }
}


//Actualiza la info que le llega por teclado
void ColaTrabajo:: InputTeclado(QString Buffer){
    QString tmp;
}


//Actualiza un campo de la tabla
void ColaTrabajo::ActualizaCampo(QString item){
    QTableWidgetItem *tmp = ui->m_Tabla->item(iFila,iColumna);
    if (!tmp){
        tmp=new QTableWidgetItem(item);
        Qt::ItemFlags Flags;
        Flags=tmp->flags();
        Flags&=~(Qt::ItemIsEditable);
        tmp->setFlags(Flags);
        ui->m_Tabla->setItem(iFila,iColumna,tmp);
    }
    else tmp->setText(item);


}


void ColaTrabajo::UpdateInfo(){

    int index=0;
    QString sql;
    int NumeroColumnasColaImpresion=NombreColumnasColaImpresion.count();
    sql=QString("select Composicion,Path,Fecha,OrdenFabricacion from ColaImpresion");
    GestorDBSQLITE DBColaImpresion;
    if (DBColaImpresion.open(m_memoryMap->m_mapa.m_database.RutaColaImpresion,"ConexionColaImpresion")!=true){
        QString tmp=tr("Error abriendo fichero de cola de impresión %1").arg(m_memoryMap->m_mapa.m_database.RutaColaImpresion);
        QMessageBox::warning(0,tr("ERROR"),tmp);
        return;
    }
    if ((DBColaImpresion.exec(sql))!=true) {
        QMessageBox::warning(0,tr("ERROR ColaImpresion (UpdateInfo)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
    }
    else{
        ui->m_Tabla->setSortingEnabled(false);

        if (DBColaImpresion.m_Tabla.count()>0){
            ui->m_Tabla->setRowCount(DBColaImpresion.m_Tabla.count()/NumeroColumnasColaImpresion);
            for (index=0;index<DBColaImpresion.m_Tabla.count();index++){
                    iFila=index/NumeroColumnasColaImpresion;
                    iColumna=index%NumeroColumnasColaImpresion;
                    ActualizaCampo(DBColaImpresion.m_Tabla.at(index));
            }
        }

        ui->m_Tabla->setSortingEnabled(true);
        ui->m_Tabla->sortItems(2,Qt::DescendingOrder);

    }
    DBColaImpresion.close();
}
