/** @addtogroup Eventos
 *  @{*/
/** @file Eventos.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de configuracion de usuarioss
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Eventos.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#include "eventos.h"
#include "configuracion.h"
#include "globals.h"
#include "teclado.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QTextEdit>
#include "GestorDB_SQLITE.h"
//#include "principal.h"
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDir>


#define DEFAULT_COUNTERS_FILE "In2Counter.sqlite"


QStringList ErroresMaquina=(QStringList()<<"Parada normal"
                            <<"Seta de emergencia"
                            <<"Fallo en variador"
                            <<"Fin de trabajo"
                            <<"Fin de bobina"
                            <<"Fallo en arrastre"
                            <<"Fallo en pulmón");

QStringList ErroresMaquinaExt1=(QStringList()<<"Seta de emergencia"
                            <<"Fallo en variador salida"
                            <<"Fallo en variador entrada"
                            <<"Fallo en arrastre"
                            <<"Bandeja nok"
                            <<"Mesa de impresión nok"
                            <<"Capota abierta"
                            <<"Presión de aire insuficiente");

Eventos::Eventos(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::GestionEventosUI)
{	
    ui->setupUi(this);
    GlobalSetScreenSize(this);
    m_memoryMap=MemoryMap::request(0);
    //this->SetDataCommon();
    m_Fila=0;
    m_Columna=0;
    QStringList nombres=GetNombreColumnasLog();
    int NumeroColumnasTablaLog=nombres.count();
    ui->m_Tabla->setColumnCount(NumeroColumnasTablaLog);
    ui->m_Tabla->setShowGrid(true);
    ui->m_Tabla->setHorizontalHeaderLabels(nombres);
    for (int index=0;index<NumeroColumnasTablaLog;index++)
            ui->m_Tabla->setColumnWidth(index,(ui->m_Tabla->width())/NumeroColumnasTablaLog);

    ui->m_Tabla->horizontalHeader()->setStretchLastSection(true);
    connect(ui->m_ResetButton,SIGNAL(clicked()),this,SLOT(ResetEventos()));
    connect(ui->m_BackButton,SIGNAL(clicked()),this,SLOT(Atras()));
    connect(ui->m_Tabla,SIGNAL(cellPressed(int,int)),this,SLOT(SeleccionCampo(int, int)));
    connect(ui->m_Tabla,SIGNAL(cellClicked(int,int)),this,SLOT(SeleccionCampo(int, int)));
    //connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));
    //connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    connect(ui->m_AceptarValores, SIGNAL(clicked()),this, SLOT(AceptarValores()));
    connect(ui->m_MostrarTodos, SIGNAL(clicked()),this, SLOT(MostrarTodos()));
    connect(ui->m_CalendarioEventos,SIGNAL(clicked(QDate)),this,SLOT(UpdateDiaMesAnoEvento()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(sltCkeckTab(int)));
    connect(ui->m_qtableLogs,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(sltGetLogTableItem(int,int)));

    #ifdef CON_TECLADOVIRTUAL
    connect(ui->m_DiaEvento,SIGNAL(selectionChanged()),this,SLOT(UpdateDiaEvento()));
    connect(ui->m_MesEvento,SIGNAL(selectionChanged()),this,SLOT(UpdateMesEvento()));
    connect(ui->m_AnoEvento,SIGNAL(selectionChanged()),this,SLOT(UpdateAnoEvento()));
    #endif

    QString dia=QDate::currentDate().toString("dd");
    ui->m_DiaEvento->setText(dia);
    QString mes=QDate::currentDate().toString("MM");
    ui->m_MesEvento->setText(mes);
    QString ano=QDate::currentDate().toString("yyyy");
    ui->m_AnoEvento->setText(ano);

    AceptarValores();

    TestRoot();

    ui->textEdit->clear();
    ui->textEdit->append("EVENTOS MAQUINA\n\n");

    int elementos;
    if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==1){
        elementos=ErroresMaquina.count();
        for(int i=0;i<elementos;i++) ui->textEdit->append(QString("%1: %2").arg(i+1).arg(ErroresMaquina.at(i)));
    }
    else if (m_memoryMap->m_mapa.m_maquina.VersionMaquina==2){
        elementos=ErroresMaquinaExt1.count();
        for(int i=0;i<elementos;i++) ui->textEdit->append(QString("Error %1: %2").arg(i).arg(ErroresMaquinaExt1.at(i)));
    }

    ui->m_qtableLogs->setColumnWidth(0,(ui->m_qtableLogs->width()*3/6));
    ui->m_qtableLogs->setColumnWidth(1,(ui->m_qtableLogs->width()*1/6));
    ui->m_qtableLogs->setColumnWidth(2,(ui->m_qtableLogs->width()*2/6));
    ui->m_qtableLogs->horizontalHeader()->setStretchLastSection(true);


    LoadLogFiles();

}
/** Captura la pulsacion de la tabla de logs*/
void Eventos::sltGetLogTableItem(int x, int y){
    if (y==0){
        QTableWidgetItem *item=ui->m_qtableLogs->item(x,y);
        if (item){
            sltShowLogFile(item->text(),x);
        }

    }
}
/** Muestra el fichero de log*/
void Eventos::sltShowLogFile(const QString &file, int x){
    QFile logFile(QString("%1\\%2\\%3").arg(QApplication::applicationDirPath()).arg(DEFAULT_INKJETDEBUG_PATH).arg(file));
    if (logFile.exists()){
        logFile.open(QIODevice::ReadOnly);
        QByteArray txt=logFile.readAll();
        logFile.close();
        //Calculamos el hash y compramos
        int posToChop=txt.lastIndexOf(HASH_ID);
        if (posToChop==-1){
            QTableWidgetItem *item=new QTableWidgetItem("NO HASH");
            ui->m_qtableLogs->setItem(x,2,item);
        }
        else{
            QByteArray fileChoped=txt.left(posToChop);

            QByteArray calculatedHash=CalculateHash(fileChoped);
            QByteArray fileToExtractHash=txt.right(txt.length()-posToChop);
            QByteArray fileHash=fileToExtractHash.right(fileToExtractHash.length()-QString("%1").arg(HASH_ID).length());
            if (fileHash==calculatedHash){
                QTableWidgetItem *item=new QTableWidgetItem("HASH OK");
                ui->m_qtableLogs->setItem(x,2,item);

            }
            else{
                QTableWidgetItem *item=new QTableWidgetItem("HASH NOK");
                ui->m_qtableLogs->setItem(x,2,item);
            }

        }
        ui->m_teShowLog->setText(QString(txt));

    }
    else{
        QMessageBox::warning(0,"ERROR","No existe el fichero",QMessageBox::Ok);
    }
}

Eventos::~Eventos()
{
}
/** Chequea el tab*/
void Eventos::sltCkeckTab(int x){
 /*   if (x==1){
        ui->m_qtableLogs->setColumnWidth(0,(ui->m_qtableLogs->width()*7/10));
        ui->m_qtableLogs->setColumnWidth(1,(ui->m_qtableLogs->width()*3/10));
        ui->m_qtableLogs->setColumnWidth(2,(ui->m_qtableLogs->width()*3/10));
        ui->m_qtableLogs->horizontalHeader()->setStretchLastSection(true);

        QLabel *msg=ShowMensaje("Loading...");
        LoadLogFiles();
        delete (msg);
    }*/
}


/** Carga los nombres de los ficheros de ese directorio*/
void Eventos::LoadLogFiles(){
    QString path=QString("%1%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_INKJETDEBUG_PATH);
    QDir dir(path);
    QStringList qstrLista=dir.entryList(QDir::Files);
    /*GestorDBSQLITE db;
    QString strPath=QString("%1\\Database\\%2").arg(QApplication::applicationDirPath()).arg(DEFAULT_COUNTERS_FILE);
    if (db.open(strPath,"Logs")){
        QString select=QString("select Files from Files");
        db.exec(select.toAscii().data());
        db.close();
    }
    for (int x=0;x<db.m_Tabla.count();x++){
        QTableWidgetItem *newItem=new QTableWidgetItem(db.m_Tabla.at(x));
        ui->m_qtableLogs->setItem(x,0,newItem);

        if (qstrLista.indexOf(db.m_Tabla.at(x))==-1){
            QTableWidgetItem *newItem=new QTableWidgetItem("NO");
            ui->m_qtableLogs->setItem(x,1,newItem);
        }
        else{
            QTableWidgetItem *newItem=new QTableWidgetItem("SI");
            ui->m_qtableLogs->setItem(x,1,newItem);
        }

    }*/




    ui->m_qtableLogs->setRowCount(qstrLista.count());
    for (int x=0;x<qstrLista.count();x++){
        QTableWidgetItem *newItem=new QTableWidgetItem(qstrLista.at(x));
        ui->m_qtableLogs->setItem(x,0,newItem);
    }
}

/** Si no es root, bloqueo funcionalidad.*/
void Eventos::TestRoot(){
    if (m_memoryMap->getUserRights()<ACCESS_ROOT){
        ui->m_DiaEvento->setEnabled(false);
        ui->m_MesEvento->setEnabled(false);
        ui->m_AnoEvento->setEnabled(false);
        ui->m_AceptarValores->setEnabled(false);
        ui->m_MostrarTodos->setEnabled(false);
    }
}
/*void Eventos::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}*/

void Eventos::Atras(){
    Configuracion *ScrConfiguracion;
    ScrConfiguracion=new Configuracion(0);
    ScrConfiguracion->show();
    this->deleteLater();
}

void Eventos::ResetEventos(){
    int respuesta=QMessageBox::question(this,"ATENCION","Desea resetear los eventos?","Si","No",QString::null, 0, 1 );
    if (respuesta==0){
        //QApplication::setStyle(new InkjetStyle(THEME_NORMAL));
        flag_led_Maquina_rojo=0;
        flag_led_Lampara_rojo=0;
    }
}

//Se selecciona un campo de la tabla
void Eventos::SeleccionCampo(int x, int y){
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow()); //Se selecciona toda la fila
    if (m_memoryMap->getUserRights()<ACCESS_ROOT){
    }
    m_Fila=x;
    m_Columna=y;
}

//Ajusta el tamaño de la tabla
void Eventos::AjustaTabla(){
}
//Actualiza la info que le llega por teclado
void Eventos:: InputTeclado(QString Buffer){
    QString tmp;
}
//Actualiza un campo de la tabla
void Eventos::ActualizaCampo(QString item){
    QTableWidgetItem *tmp = new QTableWidgetItem(item);
    Qt::ItemFlags Flags;
    Flags=tmp->flags();
    Flags&=~(Qt::ItemIsEditable);
    tmp->setFlags(Flags);
    ui->m_Tabla->setItem(m_Fila,m_Columna,tmp);
}

void Eventos::UpdateInfo(){

/*    int index=0;
    int NumeroColumnasTablaLog=GetNombreColumnasLog().count();
    QString sql;
    sql=QString("select Fecha,Usuario,Evento,Valor from Log");
    if ((miDataBaseLog.exec(sql.toAscii().data()))!=true) {
        QMessageBox::warning(0,"ERROR Eventos (UpdateInfo)","Error accediendo a base de datos",QMessageBox::Ok);
    }
    else{
        if (miDataBaseLog.m_Tabla.count()>0){
                ui->m_Tabla->setSortingEnabled(false);
                ui->m_Tabla->setRowCount(miDataBaseLog.m_Tabla.count()/NumeroColumnasTablaLog);
                for (index=0;index<miDataBaseLog.m_Tabla.count();index++){
                        m_Fila=index/NumeroColumnasTablaLog;
                        m_Columna=index%NumeroColumnasTablaLog;
                        ActualizaCampo(miDataBaseLog.m_Tabla.at(index));
                }
        }
        QStringList nombres=GetNombreColumnasLog();
        ui->m_Tabla->setHorizontalHeaderLabels(nombres);

        //ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnasTablaLog);
        ui->m_Tabla->setSortingEnabled(true);
        ui->m_Tabla->sortItems(0,Qt::DescendingOrder);
        //AjustaTabla();
    }*/
}

void Eventos::UpdateInfoFecha(QString dia, QString mes, QString ano){

    /*int index=0;
    QString sql;
    int NumeroColumnasTablaLog=GetNombreColumnasLog().count();
    QString fecha=QString("%1-%2-%3").arg(ano).arg(mes).arg(dia);

    sql=QString("select Fecha,Usuario,Evento,Valor from Log where Fecha like '%1%'").arg(fecha);
    if ((miDataBaseLog.exec(sql.toAscii().data()))!=true) {
        QMessageBox::warning(0,"ERROR Eventos (UpdateInfoFecha)","Error accediendo a base de datos",QMessageBox::Ok);
    }
    else{
        if (miDataBaseLog.m_Tabla.count()>0){
                ui->m_Tabla->setSortingEnabled(false);
                ui->m_Tabla->setRowCount(miDataBaseLog.m_Tabla.count()/NumeroColumnasTablaLog);
                for (index=0;index<miDataBaseLog.m_Tabla.count();index++){
                        m_Fila=index/NumeroColumnasTablaLog;
                        m_Columna=index%NumeroColumnasTablaLog;
                        ActualizaCampo(miDataBaseLog.m_Tabla.at(index));
                }
        }
        QStringList nombres=GetNombreColumnasLog();
        ui->m_Tabla->setHorizontalHeaderLabels(nombres);

        //ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnasTablaLog);
        ui->m_Tabla->setSortingEnabled(true);
        ui->m_Tabla->sortItems(0,Qt::DescendingOrder);
        //AjustaTabla();
    }*/
}


void Eventos::AceptarValores(){

    QString diatemp;
    QString mestemp;
    QString anotemp;
    int dummy;

    diatemp=ui->m_DiaEvento->text();
    mestemp=ui->m_MesEvento->text();
    anotemp=ui->m_AnoEvento->text();

    dummy=diatemp.size();
    if (dummy==1) {
        diatemp.prepend(QString("0"));
        ui->m_DiaEvento->setText(diatemp);
        diatemp=ui->m_DiaEvento->text();
    }

    dummy=mestemp.size();
    if (dummy==1) {
        mestemp.prepend(QString("0"));
        ui->m_MesEvento->setText(mestemp);
        mestemp=ui->m_MesEvento->text();
    }

    ui->m_Tabla->clear();

    m_Fila=0;
    m_Columna=0;

    if ((diatemp!="") && (mestemp!="") && (anotemp!=""))  UpdateInfoFecha(diatemp,mestemp,anotemp);
    else UpdateInfo();

    ui->m_BackButton->setEnabled(true);

}

void Eventos::MostrarTodos(){
    ui->m_DiaEvento->setText("");
    ui->m_MesEvento->setText("");
    ui->m_AnoEvento->setText("");

    AceptarValores();

    QString dia=QDate::currentDate().toString("dd");
    ui->m_DiaEvento->setText(dia);
    QString mes=QDate::currentDate().toString("MM");
    ui->m_MesEvento->setText(mes);
    QString ano=QDate::currentDate().toString("yyyy");
    ui->m_AnoEvento->setText(ano);

    UpdateCalendario();
}

void Eventos::UpdateDiaMesAnoEvento(){

    QDate dummy;
    int dummy_size;

    dummy=ui->m_CalendarioEventos->selectedDate();

    QString diatemp=QString("%1").arg(dummy.day());
    QString mestemp=QString("%1").arg(dummy.month());
    QString anotemp=QString("%1").arg(dummy.year());

    dummy_size=diatemp.size();
    if (dummy_size==1) {
        diatemp.prepend(QString("0"));
        ui->m_DiaEvento->setText(diatemp);
        diatemp=ui->m_DiaEvento->text();
    }

    dummy_size=mestemp.size();
    if (dummy_size==1) {
        mestemp.prepend(QString("0"));
        ui->m_MesEvento->setText(mestemp);
        mestemp=ui->m_MesEvento->text();
    }

    ui->m_DiaEvento->setText(diatemp);
    ui->m_MesEvento->setText(mestemp);
    ui->m_AnoEvento->setText(anotemp);
}

void Eventos::UpdateDiaEvento(){
    InputData *input = InputData::request(this,"Dia Evento");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateDia(QString)));
        input->setFocus();
    }
}

void Eventos::UpdateDia(QString value){
    ui->m_DiaEvento->setText(value);
    ui->m_DiaEvento->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    UpdateCalendario();
}

void Eventos::UpdateMesEvento(){
    InputData *input = InputData::request(this,"Mes Evento");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateMes(QString)));
        input->setFocus();
    }
}

void Eventos::UpdateMes(QString value){
    ui->m_MesEvento->setText(value);
    ui->m_MesEvento->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    UpdateCalendario();
}

void Eventos::UpdateAnoEvento(){
    InputData *input = InputData::request(this,"Año Evento");
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateAno(QString)));
        input->setFocus();
    }
}

void Eventos::UpdateAno(QString value){
    ui->m_AnoEvento->setText(value);
    ui->m_AnoEvento->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    UpdateCalendario();
}

void Eventos::UpdateCalendario(void){
    QDate fecha;
    int dia,mes,ano;
    QString dia_s,mes_s,ano_s;

    dia=ui->m_DiaEvento->text().toInt();
    mes=ui->m_MesEvento->text().toInt();
    ano=ui->m_AnoEvento->text().toInt();

    dia_s=QString("%1").arg(dia);
    mes_s=QString("%1").arg(mes);
    ano_s=QString("%1").arg(ano);

    fecha.setDate(ano,mes,dia);
    ui->m_CalendarioEventos->setSelectedDate(fecha);

    //UpdateInfoFecha(dia_s,mes_s,ano_s);
}



/** Cargamos datos comunes*/
/*void Eventos::SetDataCommon(){
    struct stCommon dummyDataCommon;
    dummyDataCommon.fecha=this->ui->fecha;
    //dummyDataCommon.hora=this->ui->hora;
    dummyDataCommon.led_EstadoLampara=this->ui->led_EstadoLampara;
    dummyDataCommon.led_EstadoMaquina=this->ui->led_EstadoMaquina;
    dummyDataCommon.led_EstadoTemperatura=this->ui->led_EstadoTemperatura;
    dummyDataCommon.m_Imagen=NULL;
    dummyDataCommon.m_NombreFicheroConfig=this->ui->m_NombreFicheroConfig;
    dummyDataCommon.m_RutaFicheroConfig=this->ui->m_RutaFicheroConfig;
    dummyDataCommon.m_TextoStatus=this->ui->m_TextoStatus;
    dummyDataCommon.m_User=this->ui->m_User;
    dummyDataCommon.usuario_actual=this->ui->usuario_actual;
    dummyDataCommon.m_MostrarInicio=this->ui->m_MostrarInicio;
    dummyDataCommon.m_MostrarAyuda=this->ui->m_MostrarAyuda;
    dummyDataCommon.m_MostrarMantenimiento=this->ui->m_MostrarMantenimiento;
    dummyDataCommon.lbl_EstadoLampara=ui->lblLamp;
    dummyDataCommon.lbl_EstadoMaquina=ui->lblMaquina;
    dummyDataCommon.lbl_EstadoTemperatura=ui->lblFluid;

    DataCommon=new InkjetCommon(this,dummyDataCommon);

//Actualizamos
    DataCommon->ShowInfo();
    DataCommon->ShowEstadoImpresora();
    DataCommon->ShowComms();

}*/

