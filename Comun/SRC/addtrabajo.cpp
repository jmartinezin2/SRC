#include "addtrabajo.h"
#include <QKeyEvent>
#include <QWSServer>

AddTrabajo::AddTrabajo(QWidget *parent, struct In2Trabajo t) :
    ui(new Ui::AddTrabajoUI)
{
    ui->setupUi(this);
    m_structTrabajo=t;
    //ui->m_leTrabajo->installEventFilter(this);
    ui->m_leGap->installEventFilter(this);
    ui->m_leImpreso->installEventFilter(this);
    ui->m_leTotal->installEventFilter(this);

    ui->m_leTrabajo->setText(t.strCompoFile);
    ui->m_leGap->setText(QString("%1").arg(t.iNumGaps));
    ui->m_leImpreso->setText(QString("%1").arg(t.iPrinted));
    ui->m_leTotal->setText(QString("%1").arg(t.iTotalToPrint));

    connect(ui->m_pb0,SIGNAL(clicked()),this,SLOT(sltPb0()));
    connect(ui->m_pb1,SIGNAL(clicked()),this,SLOT(sltPb1()));
    connect(ui->m_pb2,SIGNAL(clicked()),this,SLOT(sltPb2()));
    connect(ui->m_pb3,SIGNAL(clicked()),this,SLOT(sltPb3()));
    connect(ui->m_pb4,SIGNAL(clicked()),this,SLOT(sltPb4()));
    connect(ui->m_pb5,SIGNAL(clicked()),this,SLOT(sltPb5()));
    connect(ui->m_pb6,SIGNAL(clicked()),this,SLOT(sltPb6()));
    connect(ui->m_pb7,SIGNAL(clicked()),this,SLOT(sltPb7()));
    connect(ui->m_pb8,SIGNAL(clicked()),this,SLOT(sltPb8()));
    connect(ui->m_pb9,SIGNAL(clicked()),this,SLOT(sltPb9()));
    connect(ui->m_pbBack,SIGNAL(clicked()),this,SLOT(sltPbBack()));
    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltPbOk()));
    connect(ui->m_pbCancel,SIGNAL(clicked()),this,SLOT(sltPbCancel()));
    connect(ui->m_leImpreso,SIGNAL(cursorPositionChanged(int,int)),this,SLOT(sltSelectLabel(int,int)));

    m_WidgetSelected=ui->m_leImpreso;
    m_selected=false;
}

bool AddTrabajo::eventFilter(QObject *target, QEvent *event){
    if ((target==ui->m_leGap)||(target==ui->m_leImpreso)||(target==ui->m_leTotal)){
        if (event->FocusIn){
           m_WidgetSelected=static_cast<QWidget*>(target);
           m_selected=true;
        }
    }
    return QWidget::eventFilter(target,event);
}

/** Selecciona el label*/
void AddTrabajo::sltSelectLabel(int x, int y){
    m_WidgetSelected=ui->m_leImpreso;
}

/** Añade un numero*/
void AddTrabajo::addChar(QString c){
    QLineEdit *p=static_cast<QLineEdit*>(m_WidgetSelected);
    if (p){
        /*if (m_selected){
            p->setText(QString(c));
            m_selected=false;
        }
        else{
        int tmpPos=p->cursorPosition();
        QString newString=p->text();
        newString.insert(tmpPos,c.toAscii());
        p->setText(newString);
        p->setCursorPosition(tmpPos+1);
            //p->insert(QString(c));
        }*/

        int tmpPos=p->cursorPosition();
        QString newString=p->text();
        newString.insert(tmpPos,c.toAscii());
        p->setText(newString);
        p->setCursorPosition(tmpPos+1);

    }
}

/** Boton cero*/
void AddTrabajo::sltPb0(){
    addChar("0");
}

/** Boton 1*/
void AddTrabajo::sltPb1(){
    addChar("1");
}
/** Boton 2*/
void AddTrabajo::sltPb2(){
    addChar("2");
}
/** Boton 3*/
void AddTrabajo::sltPb3(){
    addChar("3");
}
/** Boton 4*/
void AddTrabajo::sltPb4(){
    addChar("4");
}
/** Boton 5*/
void AddTrabajo::sltPb5(){
    addChar("5");
}
/** Boton 6*/
void AddTrabajo::sltPb6(){
    addChar("6");
}
/** Boton 7*/
void AddTrabajo::sltPb7(){
    addChar("7");
}
/** Boton 8*/
void AddTrabajo::sltPb8(){
    addChar("8");
}
/** Boton 9*/
void AddTrabajo::sltPb9(){
    addChar("9");
}
/** Boton Back*/
void AddTrabajo::sltPbBack(){
    QLineEdit *p=static_cast<QLineEdit*>(m_WidgetSelected);
    if (p){
        int tmpPos=p->cursorPosition();
        QString newString=p->text();
        if (newString.length()>0){
            newString.remove(tmpPos-1,1);
            p->setText(newString);
            p->setCursorPosition(tmpPos+1);
        }
    }
}
/** Boton OK*/
void AddTrabajo::sltPbOk(){
    m_structTrabajo.iNumGaps=ui->m_leGap->text().toInt();
    m_structTrabajo.iPrinted=ui->m_leImpreso->text().toInt();
    m_structTrabajo.iTotalToPrint=ui->m_leTotal->text().toInt();
    emit newTrabajo(this->m_structTrabajo);
    this->deleteLater();
}
/** Boton Cancel*/
void AddTrabajo::sltPbCancel(){
    this->deleteLater();
}
