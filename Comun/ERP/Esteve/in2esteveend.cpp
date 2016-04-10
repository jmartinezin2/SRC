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
#include "in2esteveend.h"
#include "in2managererp.h"
#include "MemoryMap.h"
#include <QMessageBox>
#include "in2managerdiametros.h"
#include "monitordiametro.h"

In2EsteveEnd::In2EsteveEnd(QWidget *parent) :
    ui(new Ui::In2SGLClose){

    ui->setupUi(this);
    connect(ui->m_pbCancel,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltSaveAndExit()));
    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint);
    this->setWindowModality(Qt::ApplicationModal);

    MemoryMap *memoryMap=MemoryMap::request(0);
    int iPrinted=(memoryMap->m_mapa.m_variables.WebHeight*memoryMap->m_mapa.m_maquina.iCounter)/1000;
    int iUsed=memoryMap->m_mapa.m_maquina.metros_trabajo;
    if (iPrinted<iUsed*CORRECION_METROS) iPrinted=iUsed*CORRECION_METROS;

    if (memoryMap->m_mapa.m_maquina.metros_trabajo>=memoryMap->m_mapa.m_maquina.ref_metros_trabajo)
        ui->m_cbFinTrabajo->setChecked(true);

    ui->m_lePrinted->setText(QString("%1").arg(iPrinted));
    ui->m_leUsed->setText(QString("%1").arg(iUsed));
}

/** Decide si hay que informar al SGL o no*/
void In2EsteveEnd::setSGLUpdate(bool val){
    ui->m_cbSGLUpdate->setChecked(val);
}

/** Cerramos y salimos*/
void In2EsteveEnd::sltExit(){
    this->deleteLater();
}

/** Cerramos y salimos*/
void In2EsteveEnd::sltSaveAndExit(){
    MemoryMap *memoryMap=MemoryMap::request(0);
    //Se chequea que todo esta correcto.
    bool bPadre=ui->m_cbFinBpadre->isChecked();
    bool bHijo=ui->m_cbFinBhijo->isChecked();
    bool bRechazo=ui->m_cbrechazo->isChecked();
    bool bFinTrabajo=ui->m_cbFinTrabajo->isChecked();
    if ((!bPadre)&&(!bHijo)&&(!bRechazo)&&(!bFinTrabajo)){
        QMessageBox::warning(this,"ERROR",tr("Seleccione una causa de cierre"),QMessageBox::Ok);
        return;
    }


    if (bPadre){
        if (memoryMap->m_mapa.m_maquina.diametro_bobina_entrada>=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoEntrada){
            QMessageBox::warning(this,"ERROR",tr("Existe bulto padre, quitar antes de seguir"),QMessageBox::Ok);
            return;
        }
    }
    if (!bPadre){
        if (memoryMap->m_mapa.m_maquina.diametro_bobina_entrada<=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoEntrada){
            QMessageBox::warning(this,"ERROR",tr("No esta seleccionado Fin por Bulto Padre y no existe bulto padre"),QMessageBox::Ok);
            return;
        }
    }
    if ((bHijo)||(bRechazo)){
        if (memoryMap->m_mapa.m_maquina.diametro_bobina_salida_inst>=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoSalida){
            QMessageBox::warning(this,"ERROR",tr("Existe bulto hijo, quitar antes de seguir"),QMessageBox::Ok);
            return;
        }
    }
    if ((!bHijo)&&(!bRechazo)){
        if (memoryMap->m_mapa.m_maquina.diametro_bobina_salida_inst<=memoryMap->m_mapa.m_machinedefaults.DiametroMinimoBultoSalida){
            QMessageBox::warning(this,"ERROR",tr("No existe bulto hijo,y no esta seleccionado Fin por Rechazo o Fin por Bulto Hijo"),QMessageBox::Ok);
            return;
        }
    }


    In2ManagerERP *erp=In2ManagerERP::request(0);

    erp->setValueERPOut(out_METROS_IMPRESOS,ui->m_lePrinted->text());
    erp->setValueERPOut(out_METROS_GASTADOS,ui->m_leUsed->text());
    QString rtn;
    ui->m_cbFinBpadre->isChecked()?rtn=SGL_SI:rtn=SGL_NO;
    erp->setValueERPOut(out_IND_FIN_BULTO_PADRE,QString("%1").arg(rtn));
    ui->m_cbFinBhijo->isChecked()?rtn=SGL_SI:rtn=SGL_NO;
    erp->setValueERPOut(out_IND_FIN_BULTO_HIJO,QString("%1").arg(rtn));
    ui->m_cbrechazo->isChecked()?rtn=SGL_SI:rtn=SGL_NO;
    erp->setValueERPOut(out_IND_RECHAZO,QString("%1").arg(rtn));
    erp->setValueERPOut(out_TEXTO_OBSERVACIONES,ui->m_teComentario->toPlainText());
    erp->setValueERPOut(out_ESTADO,SGL_STATE_PND);
    rtn=SGL_NO;
    for (int x=0; x<NUM_CAMPOS;x++){
        if (memoryMap->m_mapa.m_campos[x].OnOffCampo) rtn=SGL_SI;
    }
    erp->setValueERPOut(out_IND_FORMATO_PERSONALIZADO,rtn);
    if (ui->m_cbSGLUpdate->isChecked())
        erp->writeIn2BBDD();
    else
        erp->SimulatewriteIn2BBDD();

    emit sgnERPWorkClosed();
    this->deleteLater();
}


