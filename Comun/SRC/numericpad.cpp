#include "numericpad.h"
#include "ui_numericpad.h"

NumericPad *NumericPad::mInstance  = NULL;

/** Damos la posibilidad de no usar teclado virtual*/
NumericPad *NumericPad::request(QWidget *parent)
{
#ifdef CON_TECLADOVIRTUAL
    if(mInstance == NULL){
        mInstance = new NumericPad(parent);
    }
    return mInstance;
#else
    return NULL;
#endif
}



NumericPad::NumericPad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NumericPad)
{
    ui->setupUi(this);

    Qt::WindowFlags flags=Qt::Window;
    this->setWindowFlags(flags|Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint);


    connect(ui->m_pb0,SIGNAL(clicked()),this,SLOT(sltAdd0()));
    connect(ui->m_pb1,SIGNAL(clicked()),this,SLOT(sltAdd1()));
    connect(ui->m_pb2,SIGNAL(clicked()),this,SLOT(sltAdd2()));
    connect(ui->m_pb3,SIGNAL(clicked()),this,SLOT(sltAdd3()));
    connect(ui->m_pb4,SIGNAL(clicked()),this,SLOT(sltAdd4()));
    connect(ui->m_pb5,SIGNAL(clicked()),this,SLOT(sltAdd5()));
    connect(ui->m_pb6,SIGNAL(clicked()),this,SLOT(sltAdd6()));
    connect(ui->m_pb7,SIGNAL(clicked()),this,SLOT(sltAdd7()));
    connect(ui->m_pb8,SIGNAL(clicked()),this,SLOT(sltAdd8()));
    connect(ui->m_pb9,SIGNAL(clicked()),this,SLOT(sltAdd9()));
    connect(ui->m_pbPunto,SIGNAL(clicked()),this,SLOT(sltAddPunto()));
    connect(ui->m_pbBack,SIGNAL(clicked()),this,SLOT(sltBack()));
    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltOK()));
    ui->m_leValue->setFocus();
    this->show();

}

/** Testea si existe*/
bool NumericPad::exist(){
    bool rtn=false;
    if (mInstance) rtn=true;
    return rtn;
}

/** Añade un numero*/
void NumericPad::addChar(QString c){
    QLineEdit *p=static_cast<QLineEdit*>(ui->m_leValue);
    if (p){
        int tmpPos=p->cursorPosition();
        QString newString=p->text();
        newString.insert(tmpPos,c.toAscii());
        p->setText(newString);
        p->setCursorPosition(tmpPos+1);
    }
}
/** retorcede un char*/
void NumericPad::sltBack(){
    QLineEdit *p=static_cast<QLineEdit*>(ui->m_leValue);
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

/** Añade una cifra*/
void NumericPad::sltAdd0(){
    addChar("0");
}
/** Añade una cifra*/
void NumericPad::sltAdd1(){
    addChar("1");
}
/** Añade una cifra*/
void NumericPad::sltAdd2(){
    addChar("2");
}
/** Añade una cifra*/
void NumericPad::sltAdd3(){
    addChar("3");
}
/** Añade una cifra*/
void NumericPad::sltAdd4(){
    addChar("4");
}
/** Añade una cifra*/
void NumericPad::sltAdd5(){
    addChar("5");
}
/** Añade una cifra*/
void NumericPad::sltAdd6(){
    addChar("6");
}
/** Añade una cifra*/
void NumericPad::sltAdd7(){
    addChar("7");
}
/** Añade una cifra*/
void NumericPad::sltAdd8(){
    addChar("8");
}
/** Añade una cifra*/
void NumericPad::sltAdd9(){
    addChar("9");
}
/** Añade una cifra*/
void NumericPad::sltAddPunto(){
    addChar(".");
}


/** Emite el valor para ser actualizado*/
void NumericPad::sltOK(){
    emit Update(ui->m_leValue->text());
    this->deleteLater();
}

NumericPad::~NumericPad()
{
    mInstance=NULL;
}
