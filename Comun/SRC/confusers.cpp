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
#include "confusers.h"
#include "usuarios.h"
#include "globals.h"
#include "teclado.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QScrollBar>
#include <QTextEdit>
#include "in2log.h"
//#include "principal.h"
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include "configuracion.h"

//Celda seleccionada
static int iFila=0;
static int iColumna=0;
QStringList NombreColumnasTablaUsuarios=(QStringList()<<"Nombre"<<"Password"<<"Tipo");
static unsigned char NumeroColumnasTablaUsuarios;


unsigned char flag_modificando_usuario=0;
static QString tmpNombre;
static QString tmpPassword;
static QString tmpTipo;

//Tamaños
#define TABLE_FIXED_WIDTH 16*FACTOR

ConfUsers::ConfUsers(QWidget *parent)
    : /*InkjetForm(parent)*/ui(new Ui::ConfUsersUI)
{	
    ui->setupUi(this);
    //GlobalSetScreenSize(this);
    m_memoryMap=MemoryMap::request(0);


    TipoUsuarios.append(STR_STRANGE);
    TipoUsuarios.append(STR_PRINTER);
    TipoUsuarios.append(STR_ROOT);
    TipoUsuarios.append(STR_MAINTENANCE);
    TipoUsuarios.append(STR_GOD);

    TipoUsuariosSeleccionables.append(STR_PRINTER);
    TipoUsuariosSeleccionables.append(STR_ROOT);
    TipoUsuariosSeleccionables.append(STR_MAINTENANCE);

    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    //this->setDataCommon();
    NumeroTipoUsuarios=TipoUsuarios.count();
    NumeroTipoUsuariosSeleccionables=TipoUsuariosSeleccionables.count();
	NumeroColumnasTablaUsuarios=NombreColumnasTablaUsuarios.count();

    ui->m_Tabla->setWindowTitle("Gestión de usuarios");
    ui->m_Tabla->setColumnCount(NumeroColumnasTablaUsuarios);
    ui->m_Tabla->setShowGrid(true);

    ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnasTablaUsuarios);
    for (int index=0;index<NumeroColumnasTablaUsuarios;index++)
            ui->m_Tabla->setColumnWidth(index,(180));
    ui->m_Tabla->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );
	

    connect(ui->m_BackButton,SIGNAL(clicked()),this,SLOT(Atras()));
    connect(ui->m_UsuarioNuevo,SIGNAL(clicked()),this,SLOT(UsuarioNuevo()));
    connect(ui->m_UsuarioEliminar,SIGNAL(clicked()),this,SLOT(UsuarioEliminar()));
    connect(ui->m_UsuarioModificar,SIGNAL(clicked()),this,SLOT(UsuarioModificar()));
    connect(ui->m_Tabla,SIGNAL(cellPressed(int,int)),this,SLOT(SeleccionCampo(int, int)));
    connect(ui->m_Tabla,SIGNAL(cellClicked(int,int)),this,SLOT(SeleccisetDataCommononCampo(int, int)));
    //connect(GlobalGetMainWindow(), SIGNAL(ActualizaData()),this, SLOT(ActualizaDataSlot()));
    //connect(ui->m_ButtonShowLog,SIGNAL(clicked()),GlobalGetMainWindow(),SLOT(ShowLogScreen()));

    #ifdef CON_TECLADOVIRTUAL
    //connect(ui->m_NombreUsuario, SIGNAL(selectionChanged()),this, SLOT(UpdateNombreUsuario()));
    //connect(ui->m_PasswordUsuario, SIGNAL(selectionChanged()),this, SLOT(UpdatePasswordUsuario()));
    #endif

    //connect(m_TipoUsuario, SIGNAL(activated (QString)), this, SLOT(UpdateTipoUsuario(QString seleccion)));
    connect(ui->m_AceptarValores, SIGNAL(clicked()),this, SLOT(AceptarValores()));
    connect(ui->m_CancelarValores, SIGNAL(clicked()),this, SLOT(CancelarValores()));
	
    //AbrirBaseDeDatosLog();
    UpdateInfo();

	TestRoot();
}

ConfUsers::~ConfUsers()
{
}

/** Si no es root, bloqueo funcionalidad.*/
void ConfUsers::TestRoot(){
    if (m_memoryMap->getUserRights()==ACCESS_PRINTER){
        ui->m_UsuarioNuevo->setEnabled(false);
        ui->m_UsuarioEliminar->setEnabled(false);
        ui->m_UsuarioModificar->setEnabled(false);
        ui->m_NombreUsuario->setEnabled(false);
        ui->m_PasswordUsuario->setEnabled(false);
        ui->m_TipoUsuario->setEnabled(false);
        ui->m_AceptarValores->setEnabled(false);
        ui->m_CancelarValores->setEnabled(false);
    }
    else if (m_memoryMap->getUserRights()==ACCESS_MAINTENANCE){
        ui->m_UsuarioNuevo->setEnabled(false);
        ui->m_UsuarioEliminar->setEnabled(false);
        ui->m_UsuarioModificar->setEnabled(false);
        ui->m_NombreUsuario->setEnabled(false);
        ui->m_PasswordUsuario->setEnabled(false);
        ui->m_TipoUsuario->setEnabled(false);
        ui->m_AceptarValores->setEnabled(false);
        ui->m_CancelarValores->setEnabled(false);
    }

    else {
        ui->m_UsuarioNuevo->setEnabled(true);
        ui->m_UsuarioEliminar->setEnabled(true);
        ui->m_UsuarioModificar->setEnabled(true);
        ui->m_NombreUsuario->setEnabled(true);
        ui->m_PasswordUsuario->setEnabled(true);
        ui->m_TipoUsuario->setEnabled(true);
        ui->m_AceptarValores->setEnabled(true);
        ui->m_CancelarValores->setEnabled(true);


    }
}
/*void ConfUsers::ActualizaDataSlot(){
    DataCommon->ShowComms();
    DataCommon->ShowEstadoImpresora();
}*/

void ConfUsers::Atras(){
    /*Usuarios *ScrUsuarios;
    ScrUsuarios=new Usuarios(0);
    ScrUsuarios->show();*/
    this->deleteLater();
}

//Elimina la usuario seleccionado
void ConfUsers::UsuarioEliminar(){
    QString tmp;
    QTableWidgetItem *aux;
    int FilaSeleccionada=ui->m_Tabla->currentRow();
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());
    if (FilaSeleccionada!=-1){
        int ret = QMessageBox::warning(this, tr("INFO"),tr("Desea eliminar el usuario seleccionado?"),
        QMessageBox::Ok | QMessageBox::Cancel);
        if (ret==QMessageBox::Ok){
                //aux=new QTableWidgetItem();
                aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
                tmp.sprintf("delete from Usuarios where %s=\"%s\"",NombreColumnasTablaUsuarios.at(0).toAscii().data(),aux->text().toAscii().data());
                ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
                In2Log db;
                if (!db.execSQLNoSave(tmp))
                    QMessageBox::warning(0,tr("ERROR ConfUsers (UsuarioEliminar)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
                UpdateInfo();
        }
    }
}

//Elimina el usuario seleccionado
void ConfUsers::UsuarioEliminar_SinPreguntar(){

    QString tmp;
    ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
    tmp=QString("delete from Usuarios where Nombre='%1'")
                .arg(tmpNombre);
    In2Log db;
    if (!db.execSQLNoSave(tmp))
        QMessageBox::warning(0,tr("ERROR ConfUsers (UsuarioEliminar_SinPreguntar)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);

}

//Modifica la usuario seleccionado
void ConfUsers::UsuarioModificar(){

    int FilaSeleccionada=ui->m_Tabla->currentRow();
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());
    if (FilaSeleccionada!=-1){
        CargarValoresTipo();

        ui->m_BackButton->setEnabled(false);
        ui->m_UsuarioNuevo->setEnabled(false);
        ui->m_UsuarioEliminar->setEnabled(false);
        ui->m_UsuarioModificar->setEnabled(false);

        ui->m_NombreUsuario->setEnabled(true);
        ui->m_PasswordUsuario->setEnabled(true);
        ui->m_TipoUsuario->setEnabled(true);
        ui->m_AceptarValores->setEnabled(true);
        ui->m_CancelarValores->setEnabled(true);

        QTableWidgetItem *aux;

        ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());

        //aux=new QTableWidgetItem();

        aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
        tmpNombre.sprintf(aux->text().toAscii().data());
        ui->m_NombreUsuario->setText(tmpNombre);

        aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),1);
        tmpPassword.sprintf(aux->text().toAscii().data());
        ui->m_PasswordUsuario->setText(tmpPassword);

        aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),2);
        tmpTipo.sprintf(aux->text().toAscii().data());
        for (int i=0;i<NumeroTipoUsuariosSeleccionables;i++){
           if (tmpTipo==TipoUsuariosSeleccionables.at(i).toAscii().data()) ui->m_TipoUsuario->setCurrentIndex(i);
        }
        flag_modificando_usuario=1;
        ui->m_UsuarioModificar->setEnabled(false);
        ui->m_UsuarioEliminar->setEnabled(false);

    }

}


//Carga un nuevo usuario
void ConfUsers::UsuarioNuevo(){
    CargarValoresTipo();
    //ui->m_OpcionesUsuarios->setEnabled(true);
    ui->m_BackButton->setEnabled(false);
    ui->m_UsuarioNuevo->setEnabled(false);
    ui->m_UsuarioEliminar->setEnabled(false);
    ui->m_UsuarioModificar->setEnabled(false);

    ui->m_NombreUsuario->setEnabled(true);
    ui->m_PasswordUsuario->setEnabled(true);
    ui->m_TipoUsuario->setEnabled(true);
    ui->m_AceptarValores->setEnabled(true);
    ui->m_CancelarValores->setEnabled(true);

}

//Se selecciona un campo de la tabla
void ConfUsers::SeleccionCampo(int x, int y){
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow()); //Se selecciona toda la fila
    if (m_memoryMap->getUserRights()>ACCESS_PRINTER){
        ui->m_UsuarioEliminar->setEnabled(true);
        ui->m_UsuarioModificar->setEnabled(true);
    }
    iFila=x;
    iColumna=y;
}

//Ajusta el tamaño de la tabla
void ConfUsers::AjustaTabla(){
	int newWidth=0;
	int newHeight=0;
	int i;

	for(i=0;i<NumeroColumnasTablaUsuarios;i++){
        newWidth+= ui->m_Tabla->columnWidth(i);
        }
    newWidth+=ui->m_Tabla->verticalHeader()->width() + 2 * ui->m_Tabla->frameWidth();
	
    newHeight=ui->m_Tabla->horizontalHeader()->height()+(ui->m_Tabla->rowCount())*ui->m_Tabla->rowHeight(0)+2*ui->m_Tabla->frameWidth();
	
	if (newHeight>HTABLA) {
		newHeight=HTABLA;
        ui->m_Tabla->verticalScrollBar()->setFixedWidth(TABLE_FIXED_WIDTH);
        for (int index=0;index<NumeroColumnasTablaUsuarios;index++) ui->m_Tabla->setColumnWidth(index,(WTABLA)/NumeroColumnasTablaUsuarios-1);
		newWidth=0;
		for(i=0;i<NumeroColumnasTablaUsuarios;i++){
            newWidth+= ui->m_Tabla->columnWidth(i);
		}
        newWidth+=ui->m_Tabla->verticalHeader()->width() + 2 * ui->m_Tabla->frameWidth();
        newWidth+=ui->m_Tabla->verticalScrollBar()->width();	//Lo pongo a mano porque el verticalScrollBar()->width me devuelve 100??????
	}
    ui->m_Tabla->setMaximumWidth(newWidth);
    ui->m_Tabla->setMinimumWidth(newWidth);
    //ui->m_Tabla->setMaximumHeight(200);
    //ui->m_Tabla->setGeometry(4*DELTA,4*DELTA/*(IMG_HEIGHT-HTABLA)/2*/,newWidth,newHeight);
}
//Actualiza la info que le llega por teclado
void ConfUsers:: InputTeclado(QString Buffer){
    QString tmp;
}
//Actualiza un campo de la tabla
void ConfUsers::ActualizaCampo(QString item){	
    bool newItem=false;
    QTableWidgetItem *tmp=ui->m_Tabla->item(iFila,iColumna);
    if (!tmp) {
        tmp=new QTableWidgetItem(item);
        newItem=true;
    }
    Qt::ItemFlags Flags;
    Flags=tmp->flags();
    Flags&=~(Qt::ItemIsEditable);
    tmp->setFlags(Flags);
    if (newItem)
        ui->m_Tabla->setItem(iFila,iColumna,tmp);
}

void ConfUsers::UpdateInfo(){

    int index=0;
    QString sql;

    iFila=0;
    iColumna=0;

    ui->m_Tabla->clear();
    ui->m_Tabla->setSortingEnabled(false);



    if (m_memoryMap->getUserRights()>=ACCESS_ROOT) sql=QString("select Nombre,Password,Tipo from Usuarios");
    else sql=QString("select Nombre,Password,Tipo from Usuarios where Nombre='%1'").arg(m_memoryMap->getUser());

    In2Log db;
    QStringList rslt=db.execSQL(sql);
    if (!rslt.count()) {
        QMessageBox::warning(0,tr("ERROR ConfUsers (UpdateInfo)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
    }
    else{
        ui->m_Tabla->setRowCount(rslt.count()/NumeroColumnasTablaUsuarios);
        for (index=0;index<rslt.count();index++){
            iFila=index/NumeroColumnasTablaUsuarios;
            iColumna=index%NumeroColumnasTablaUsuarios;
            if (iColumna==1){
                if (m_memoryMap->getUserRights()==ACCESS_GOD)
                    ActualizaCampo(rslt.at(index));
                else
                    ActualizaCampo("*****");
            }
            else
                ActualizaCampo(rslt.at(index));
        }
        ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnasTablaUsuarios);
        ui->m_Tabla->setSortingEnabled(true);
        ui->m_Tabla->sortItems(0,Qt::AscendingOrder);
       // AjustaTabla();
    }
}


void ConfUsers::UpdateNombreUsuario(){    
    //InputData *input = InputData::request(this,"Nombre Usuario");
    QString text=ui->m_NombreUsuario->toPlainText();
    InputData *input = InputData::request(this,"Nombre Usuario",text);
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateNombre(QString)));
        input->setFocus();
    }
}
void ConfUsers::UpdateNombre(QString value){
    ui->m_NombreUsuario->setText(value);
    ui->m_NombreUsuario->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void ConfUsers::UpdatePasswordUsuario(){
    //InputData *input = InputData::request(this,"Password Usuario");
    QString text=ui->m_PasswordUsuario->toPlainText();
    InputData *input = InputData::request(this,"Password Usuario",text);
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdatePassword(QString)));
        input->setFocus();
    }
}
void ConfUsers::UpdatePassword(QString value){
        ui->m_PasswordUsuario->setText(value);
        ui->m_PasswordUsuario->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}



void ConfUsers::CancelarValores(){

	flag_modificando_usuario=0;
    ui->m_NombreUsuario->setText("");
    ui->m_PasswordUsuario->setText("");
    ///ui->m_OpcionesUsuarios->setEnabled(false);
    ui->m_BackButton->setEnabled(true);
    ui->m_UsuarioNuevo->setEnabled(true);
    ui->m_UsuarioEliminar->setEnabled(true);
    ui->m_UsuarioModificar->setEnabled(true);
    ui->m_TipoUsuario->clear();
    ui->m_NombreUsuario->setEnabled(false);
    ui->m_PasswordUsuario->setEnabled(false);
    ui->m_TipoUsuario->setEnabled(false);
    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);
}
/** Testea la bondad del password*/
int ConfUsers::TestPasswordStrength(){
    int rtn=PASSWORD_POLICY_OK;
    QString tmpPass=ui->m_PasswordUsuario->toPlainText();
    if (tmpPass.length()<PASSWORD_POLICY_LEN) rtn=PASSWORD_POLICY_LEN;
    else{
        QRegExp exp("[a-z]");
        int x=exp.indexIn(tmpPass);
        if (x>=0){
            exp=QRegExp("[0-9]{1,1}");
            x=exp.indexIn(tmpPass);
            if (x<0){
                rtn=PASSWORD_NUMBERS_AND_CHARS;
            }
        }
        else
            rtn=PASSWORD_NUMBERS_AND_CHARS;
    }
    return rtn;
}

//Acepta la seleccion
void ConfUsers::InsertarValores(){
    if (TestPasswordStrength()!=PASSWORD_POLICY_OK){
        QString txt=QString("Password debe estar formado por letras y numeros y una longitud minima de %1 caracteres").arg(PASSWORD_POLICY_LEN);
        QMessageBox::warning(this,"ERROR",txt,QMessageBox::Ok);
        return;
    }
    QString tmp;
    QByteArray dummy,dummy_hash;
    dummy.clear();
    dummy.append(ui->m_NombreUsuario->toPlainText().toAscii().data());
    dummy.append(ui->m_TipoUsuario->currentText().toAscii().data());
    dummy.append(ui->m_PasswordUsuario->toPlainText().toAscii().data());
    //qDebug() << "Datos para hash: " << dummy;
    dummy_hash=CalculateHash(dummy);

    tmp=QString("insert into Usuarios ('Nombre','Tipo','Password','Hash') Values ('%1','%2','%3','%4')")
            .arg(ui->m_NombreUsuario->toPlainText().toAscii().data())
            .arg(ui->m_TipoUsuario->currentText().toAscii().data())
            .arg(ui->m_PasswordUsuario->toPlainText().toAscii().data())
            .arg(dummy_hash.constData());

    In2Log db;
    if (!db.execSQLNoSave(tmp))
        QMessageBox::warning(0,tr("ERROR ConfUsers (InsertarValores)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);

    QTimer::singleShot(0,this,SLOT(UpdateInfo()));

}

void ConfUsers::AceptarValores(){

    if (flag_modificando_usuario==0){
        QString tmpUser=ui->m_NombreUsuario->toPlainText();
        bool bUserExists=false;
        int iTablaLen=ui->m_Tabla->rowCount();
        for (int x=0;x<iTablaLen;x++){
            QTableWidgetItem *aux=ui->m_Tabla->item(x,0);
            if (!aux->text().compare(tmpUser)){
               bUserExists=true;
               break;
            }
        }
        if (bUserExists){
            QMessageBox::warning(this,tr("ERROR"),tr("ERROR, EL USUARIO YA EXISTE"),QMessageBox::Ok);
            return;
        }
        InsertarValores();
    }
    else{
        UsuarioEliminar_SinPreguntar();
        InsertarValores();
    }

    UpdateInfo();
    flag_modificando_usuario=0;
    ui->m_NombreUsuario->setText("");
    ui->m_PasswordUsuario->setText("");
    ui->m_BackButton->setEnabled(true);
    ui->m_UsuarioNuevo->setEnabled(true);
    ui->m_UsuarioEliminar->setEnabled(true);
    ui->m_UsuarioModificar->setEnabled(true);

    ui->m_NombreUsuario->setEnabled(false);
    ui->m_PasswordUsuario->setEnabled(false);
    ui->m_TipoUsuario->setEnabled(false);
    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);

    ui->m_TipoUsuario->clear();

}


void ConfUsers::CargarValoresTipo(){
	
    QString Vtemp;

    for (int i=0;i<NumeroTipoUsuariosSeleccionables;i++){
        Vtemp=TipoUsuariosSeleccionables.at(i).toAscii().data();
        ui->m_TipoUsuario->addItem(Vtemp);
    }

}

/** Cargamos datos comunes*/
/*void ConfUsers::setDataCommon(){
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
