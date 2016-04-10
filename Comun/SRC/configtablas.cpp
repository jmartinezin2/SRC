#include "configtablas.h"
#include "MemoryMap.h"
#include "globals.h"
#include "inputdata.h"
#include <QMessageBox>
#include "in2log.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

#define GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA        0
#define GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA_2C     1


ConfigTablas::ConfigTablas(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigTablasUI)
{
    ui->setupUi(this);
    GlobalSetScreenSize(this);
    setDataCommon();

    iFila=0;
    iColumna=0;
    flag_modificando_xy=0;
    NombreColumnasTablaRefVelocidadIntensidad=(QStringList()<<"RefVelocidad"<<"Intensidad");
    NombreColumnasTablaRefVelocidadIntensidad2C=(QStringList()<<"RefVelocidad"<<"Intensidad");
    ui->m_Tabla->horizontalHeader()->setResizeMode( 0, QHeaderView::Stretch );

    connect(ui->m_Aceptar,SIGNAL(clicked()),this,SLOT(Salir()));
    connect(ui->m_NuevoPunto,SIGNAL(clicked()),this,SLOT(RegistroNuevo()));
    connect(ui->m_ModificarPunto,SIGNAL(clicked()),this,SLOT(RegistroModificar()));
    connect(ui->m_EliminarPunto,SIGNAL(clicked()),this,SLOT(RegistroEliminar()));

    connect(ui->m_TipoCurva,SIGNAL(activated(QString)),this,SLOT(CambioTipoCurva()));
    connect(ui->m_AceptarValores,SIGNAL(clicked()),this,SLOT(InsertarValores()));
    connect(ui->m_CancelarValores,SIGNAL(clicked()),this,SLOT(CancelarValores()));
    //connect(ui->m_Tabla,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(sltCellSelected(int,int)));

    connect(this, SIGNAL(SendDebugInfo(unsigned char,QString)),GlobalGetMainWindow(), SLOT(ReceiveDebugInfo(unsigned char,QString)));

    #ifdef CON_TECLADOVIRTUAL
    connect(ui->m_x, SIGNAL(selectionChanged()),this, SLOT(UpdateXPunto()));
    connect(ui->m_y, SIGNAL(selectionChanged()),this, SLOT(UpdateYPunto()));
    #endif

    ui->m_NuevoPunto->setEnabled(true);
    ui->m_ModificarPunto->setEnabled(true);
    ui->m_EliminarPunto->setEnabled(true);

    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);
    ui->m_x->setEnabled(false);
    ui->m_y->setEnabled(false);

    ui->m_TipoCurva->clear();
    ui->m_TipoCurva->addItem("Ref Velocidad - Intensidad lámpara");
    ui->m_TipoCurva->addItem("Ref Velocidad - Intensidad lámpara 2 Colores");
    ui->m_TipoCurva->setCurrentIndex(0);

    NumeroColumnasTablaRefVelocidadIntensidad=NombreColumnasTablaRefVelocidadIntensidad.count();
    NumeroColumnasTablaRefVelocidadIntensidad2C=NombreColumnasTablaRefVelocidadIntensidad2C.count();

    grid = new QwtPlotGrid();
    curve = new QwtPlotCurve();
    symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::white ), QPen( Qt::blue, 2 ), QSize( 8, 8 ) );

    UpdateInfo();
    UpdateGrafico();

    connect(ui->m_Tabla,SIGNAL(cellChanged(int,int)),this,SLOT(sltActualiza(int,int)));
    connect(ui->m_Tabla,SIGNAL(cellPressed(int,int)),this,SLOT(sltCellSelected(int,int)));

}
/** Selecciona una celda*/
void ConfigTablas::sltCellSelected(int x,int y){
    QTableWidgetItem *tmpX=ui->m_Tabla->item(0,y);
    if (tmpX){
        ui->m_x->setText(tmpX->text());
    }
    QTableWidgetItem *tmpY=ui->m_Tabla->item(1,y);
    if (tmpY){
        ui->m_y->setText(tmpY->text());
    }

}

/** Actualiza grafico*/
void ConfigTablas::sltActualiza(int x,int y){
    InsertarValores();
    //Introducir_puntos();
}

ConfigTablas::~ConfigTablas()
{
    delete ui;
}

/** Salimos*/
void ConfigTablas::Salir(){
    this->deleteLater();
}

/** Update grafico*/
void ConfigTablas::UpdateGrafico(){

    ui->frameGrafico->setCanvasBackground( Qt::white );

    TipoGrafico=ui->m_TipoCurva->currentIndex();

    QFont fuente;
    fuente.setBold(true);
    fuente.setPointSize(8);

    switch (TipoGrafico){
    case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA:
        ui->lbl_x->setText("X [m/min]");
        ui->lbl_y->setText("Y [%]");
        ui->frameGrafico->setAxisScale(QwtPlot::yLeft, 0.0, 100.0 );
        ui->frameGrafico->setAxisTitle(QwtPlot::xBottom,QString("<font size=2> Ref Velocidad [m/min] </font>"));
        ui->frameGrafico->setAxisTitle(QwtPlot::yLeft,QString("<font size=2> Intensidad Lámpara [%] </font>"));
        break;
    case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA_2C:
        ui->lbl_x->setText("X [m/min]");
        ui->lbl_y->setText("Y [%]");
        ui->frameGrafico->setAxisScale(QwtPlot::yLeft, 0.0, 100.0 );
        ui->frameGrafico->setAxisTitle(QwtPlot::xBottom,QString("<font size=2> Ref Velocidad 2C [m/min] </font>"));
        ui->frameGrafico->setAxisTitle(QwtPlot::yLeft,QString("<font size=2> Intensidad Lámpara 2C [%] </font>"));
        break;
    default:
        break;
    }

    ui->frameGrafico->setAxisFont(QwtPlot::xBottom,fuente);
    ui->frameGrafico->setAxisFont(QwtPlot::yLeft,fuente);

    grid->attach( ui->frameGrafico );

    curve->setPen( QPen( Qt::red, 4 ) ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->setSymbol( symbol );

    UpdateCurva();

    curve->attach( ui->frameGrafico );

}

void ConfigTablas::CambioTipoCurva(){
    UpdateInfo();
    UpdateGrafico();
}

void ConfigTablas::UpdateInfo(){

    int index=0;
    QString sql;

    iFila=0;
    iColumna=0;    
    ui->m_Tabla->clear();
    //ui->m_Tabla->setSortingEnabled(false);
    TipoGrafico=ui->m_TipoCurva->currentIndex();

    switch (TipoGrafico){
        case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA:
            sql=QString("select * from CurvaLampara order by RefVelocidad");
            NumeroColumnas=NumeroColumnasTablaRefVelocidadIntensidad;
            NombreColumnas=NombreColumnasTablaRefVelocidadIntensidad;
            break;
        case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA_2C:
            sql=QString("select * from CurvaLampara2C order by RefVelocidad");
            NumeroColumnas=NumeroColumnasTablaRefVelocidadIntensidad2C;
            NombreColumnas=NombreColumnasTablaRefVelocidadIntensidad2C;
            break;
    }

    emit SendDebugInfo(LOGGER_WARNING, QString("Sql Tabla velocidad %1").arg(sql));

    In2Log db;
    QStringList rslt=db.execSQL(sql);
    if (rslt.isEmpty()) {
        QMessageBox::warning(0,tr("ERROR ConfigTablas (UpdateInfo)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
        emit SendDebugInfo(LOGGER_ERROR, QString("ERROR Sql %1").arg(sql));
    }
    else{        
        ui->m_Tabla->setRowCount(rslt.count()/NumeroColumnas);
        for (index=0;index<rslt.count();index++){
            iFila=index/NumeroColumnas;
            iColumna=index%NumeroColumnas;
            ActualizaCampo(rslt.at(index));
        }
        ui->m_Tabla->setHorizontalHeaderLabels(NombreColumnas);
    }
}

//Actualiza un campo de la tabla
void ConfigTablas::ActualizaCampo(QString item){
    QTableWidgetItem *tmp =ui->m_Tabla->item(iFila,iColumna);
    if (!tmp){
            tmp=new QTableWidgetItem(item);
            ui->m_Tabla->setItem(iFila,iColumna,tmp);
    }
    else{
        tmp->setText(item);
    }
}



//Elimina la usuario seleccionado
void ConfigTablas::RegistroEliminar(){

    QString tmp;
    QTableWidgetItem *aux0,*aux1;
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());
    int FilaSeleccionada=ui->m_Tabla->currentRow();

    if (FilaSeleccionada!=-1){
        int ret = QMessageBox::warning(this, tr("ATENCIÓN"),tr("¿Desea eliminar el punto seleccionado?"),
        QMessageBox::Ok | QMessageBox::Cancel);
        if (ret==QMessageBox::Ok){
            aux0=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
            aux1=ui->m_Tabla->item(ui->m_Tabla->currentRow(),1);

            switch (TipoGrafico){
                case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA:
                    tmp=QString("delete from CurvaLampara where RefVelocidad='%1' and Intensidad='%2' ")
                            .arg(aux0->text().toAscii().data())
                            .arg(aux1->text().toAscii().data());
                    break;
                case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA_2C:
                    tmp=QString("delete from CurvaLampara2C where RefVelocidad='%1' and Intensidad='%2' ")
                        .arg(aux0->text().toAscii().data())
                        .arg(aux1->text().toAscii().data());
                    break;
            }

            emit SendDebugInfo(LOGGER_WARNING, QString("Sql Tabla velocidad %1").arg(tmp));

            In2Log db;
            if (!db.execSQLNoSave(tmp)){
                QMessageBox::warning(0,tr("ERROR ConfigTablas (RegistroEliminar)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
                emit SendDebugInfo(LOGGER_ERROR, QString("ERROR Sql %1").arg(tmp));
            }
            else{
               ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
            }
        }
    }

    //Introducir_puntos();//UpdateGrafico();


}



//Modifica la usuario seleccionado
void ConfigTablas::RegistroModificar(){


    RegistroNuevo();

    QTableWidgetItem *aux;
    ui->m_Tabla->selectRow(ui->m_Tabla->currentRow());

    aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),0);
    tmpx.sprintf(aux->text().toAscii().data());
    ui->m_x->setText(tmpx);

    aux=ui->m_Tabla->item(ui->m_Tabla->currentRow(),1);
    tmpy.sprintf(aux->text().toAscii().data());
    ui->m_y->setText(tmpy);

    flag_modificando_xy=1;


}

void ConfigTablas::RegistroNuevo(){
    ui->m_NuevoPunto->setEnabled(false);
    ui->m_ModificarPunto->setEnabled(false);
    ui->m_EliminarPunto->setEnabled(false);
    ui->m_Aceptar->setEnabled(false);

    ui->m_AceptarValores->setEnabled(true);
    ui->m_CancelarValores->setEnabled(true);
    ui->m_x->setEnabled(true);
    ui->m_y->setEnabled(true);

}

//Acepta la seleccion
void ConfigTablas::InsertarValores(){

    QString tmp;

    if (flag_modificando_xy==1) Eliminar_SinPreguntar();

    switch (TipoGrafico){
        case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA:
            tmp=QString("insert into CurvaLampara ('RefVelocidad','Intensidad') Values ('%1','%2')")
                .arg(ui->m_x->toPlainText().toAscii().data())
                .arg(ui->m_y->toPlainText().toAscii().data());
            break;
        case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA_2C:
            tmp=QString("insert into CurvaLampara2C ('RefVelocidad','Intensidad') Values ('%1','%2')")
                .arg(ui->m_x->toPlainText().toAscii().data())
                .arg(ui->m_y->toPlainText().toAscii().data());
            break;
    }

    emit SendDebugInfo(LOGGER_WARNING, QString("Sql Tabla velocidad %1").arg(tmp));

    In2Log db;
    if (!db.execSQLNoSave(tmp)){
        emit SendDebugInfo(LOGGER_ERROR, QString("ERROR Sql %1").arg(tmp));
    }
    else{
        //UpdateInfo();
        //Introducir_puntos();//UpdateGrafico();
        switch (TipoGrafico){
            case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA:
                //RefV_Intensidad=xy_punto;
                break;
            case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA_2C:
                //RefV_Intensidad_2C=xy_punto;
                break;
        }

    }

    ui->m_NuevoPunto->setEnabled(true);
    ui->m_ModificarPunto->setEnabled(true);
    ui->m_EliminarPunto->setEnabled(true);
    ui->m_Aceptar->setEnabled(true);

    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);
    ui->m_x->setText("");
    ui->m_y->setText("");
    ui->m_x->setEnabled(false);
    ui->m_y->setEnabled(false);
}

void ConfigTablas::CancelarValores(){
    ui->m_NuevoPunto->setEnabled(true);
    ui->m_ModificarPunto->setEnabled(true);
    ui->m_EliminarPunto->setEnabled(true);
    ui->m_Aceptar->setEnabled(true);

    ui->m_AceptarValores->setEnabled(false);
    ui->m_CancelarValores->setEnabled(false);
    ui->m_x->setText("");
    ui->m_y->setText("");
    ui->m_x->setEnabled(false);
    ui->m_y->setEnabled(false);
}

//Elimina el usuario seleccionado
void ConfigTablas::Eliminar_SinPreguntar(){

    QString tmp;
    ui->m_Tabla->removeRow(ui->m_Tabla->currentRow());
    switch (TipoGrafico){
        case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA:
            tmp=QString("delete from CurvaLampara where RefVelocidad='%1'")
                    .arg(tmpx);
            break;
        case GRAFICA_REF_VELOCIDAD_INTENSIDAD_LAMPARA_2C:
            tmp=QString("delete from CurvaLampara2C where RefVelocidad='%1'")
                    .arg(tmpx);
            break;
    }


    emit SendDebugInfo(LOGGER_WARNING, QString("Sql Tabla velocidad %1").arg(tmp));
    In2Log db;
    if (!db.execSQLNoSave(tmp)){
        QMessageBox::warning(0,tr("ERROR ConfigTablas (Eliminar_SinPreguntar)"),tr("Error accediendo a base de datos"),QMessageBox::Ok);
        emit SendDebugInfo(LOGGER_ERROR, QString("ERROR Sql %1").arg(tmp));
    }
}

void ConfigTablas::UpdateCurva(){

    QPolygonF points;

    int num=ui->m_Tabla->rowCount();

    for (int x=0;x<num;x++){
        QTableWidgetItem *vel=ui->m_Tabla->item(x,0);
        QTableWidgetItem *inten=ui->m_Tabla->item(x,0);
        float fvel=0,fint=0;
        if (vel){
            fvel=vel->text().toFloat();
        }
        if (inten){
            fint=inten->text().toFloat();
        }
        points << QPointF(fvel,fint );
    }

    curve->setSamples( points );

    ui->frameGrafico->replot();
}


void ConfigTablas::UpdateXPunto(){
    QString text=ui->m_x->toPlainText();
    InputData *input = InputData::request(this,"X Punto",text);
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateX(QString)));
        input->setFocus();
    }
}

void ConfigTablas::UpdateX(QString value){
    ui->m_x->setText(value);
    ui->m_x->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

void ConfigTablas::UpdateYPunto(){
    QString text=ui->m_y->toPlainText();
    InputData *input = InputData::request(this,"Y Punto",text);
    if (input){
        connect(input,SIGNAL(Update(QString)),this,SLOT(UpdateY(QString)));
        input->setFocus();
    }
}

void ConfigTablas::UpdateY(QString value){
    ui->m_y->setText(value);
    ui->m_y->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

/** Cargamos datos comunes*/
void ConfigTablas::setDataCommon(){
}
