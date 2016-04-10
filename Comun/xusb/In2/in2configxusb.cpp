#include "in2configxusb.h"
#include "ui_in2configxusb.h"
#include "gestorimpresion.h"
#include "in2xmlparser.h"
#include <QTimer>
#include <QMessageBox>


//0: = No Head, 1: = Boot in Progress, 2: = Running,
//3: = Failed boot, 4: = TC Error, 5: = Parameter error

//factor de calculo de encoder interno
#define FACTOR_ENCODER 2.98

In2ConfigXUSB::In2ConfigXUSB(QWidget *parent,QString strConfigFile) :
    QDialog(parent),
    ui(new Ui::In2ConfigXUSB)
{
    ui->setupUi(this);
    m_strConfigFile=strConfigFile;
    m_card=0;
    m_parameters=new CPrintParameters(0,m_card,m_strConfigFile);

    connect(ui->m_pbOK,SIGNAL(clicked()),this,SLOT(sltSaveData()));
    connect(ui->m_pbCancel,SIGNAL(clicked()),this,SLOT(deleteLater()));
    connect(ui->m_cbHAll1,SIGNAL(toggled(bool)),this,SLOT(sltEnableAllRow1(bool)));
    connect(ui->m_cbHAll2,SIGNAL(toggled(bool)),this,SLOT(sltEnableAllRow2(bool)));
    connect(ui->m_leGOffsetpuntos,SIGNAL(editingFinished()),this,SLOT(sltUpdateGOffsetmm()));
    connect(ui->m_leGOffsetmm,SIGNAL(editingFinished()),this,SLOT(sltUpdateGOffsetpuntos()));

    In2XmlParser parser(0,m_strConfigFile);
    QStringList params=QStringList()<<"XUSB"<<"Configuration"<<"PrintMode";
    QString val=parser.getValue(params,"value");
    ui->m_cbImpresion->setCurrentIndex(val.toInt());

    params=QStringList()<<"XUSB"<<"Common"<<"CopyCount";
    val=parser.getValue(params,"value");
    ui->m_leCopias->setText(val);

    params=QStringList()<<"XUSB"<<"Common"<<"EncoderResolution";
    val=parser.getValue(params,"value");
    ui->m_leEncoderRes->setText(val);

    params=QStringList()<<"XUSB"<<"Common"<<"TransportSpeed";
    val=parser.getValue(params,"value");
    ui->m_leTransportSpeed->setText(val);

    params=QStringList()<<"XUSB"<<"Common"<<"RequiredDPI";
    val=parser.getValue(params,"value");
    ui->m_leRequiredDPI->setText(val);

    params=QStringList()<<"XUSB"<<"Configuration"<<"DDFSValue";
    val=parser.getValue(params,"value");
    float x=val.toFloat()*FACTOR_ENCODER;
    ui->m_leFreqEncoderInterno->setText(QString("%1").arg(x));

    params=QStringList()<<"XUSB"<<"HeadOffsetRegistersCard1"<<"GlobalOffset";
    val=parser.getValue(params,"value");
    ui->m_leGOffsetpuntos->setText(val);
    sltUpdateGOffsetmm();

    params=QStringList()<<"XUSB"<<"Configuration"<<"SequenceStart";
    val=parser.getValue(params,"value");
    ui->m_cbTipoPD->setCurrentIndex(val.toInt());

    params=QStringList()<<"XUSB"<<"Configuration"<<"SysClock";
    val=parser.getValue(params,"value");
    ui->m_cbTipoEncoder->setCurrentIndex(val.toInt());




    QTimer::singleShot(0,this,SLOT(sltTestHeads()));

}
/** Actualiza el offset en puntos*/
void In2ConfigXUSB::sltUpdateGOffsetpuntos(){
    float x=ui->m_leGOffsetmm->text().toFloat()*FACTOR_PIXELS_MM;
    ui->m_leGOffsetpuntos->setText(QString("%1").arg((int)x));
}
/** Actualiza el offset en mm*/
void In2ConfigXUSB::sltUpdateGOffsetmm(){
    float x=ui->m_leGOffsetpuntos->text().toFloat()/FACTOR_PIXELS_MM;
    ui->m_leGOffsetmm->setText(QString("%1").arg(x));
}


In2ConfigXUSB::~In2ConfigXUSB()
{
    delete (m_parameters);
    delete ui;
}

/** Des/habilita todas las row 0*/
void In2ConfigXUSB::sltEnableAllRow1(bool enable){
    ui->m_cbH11->setChecked(enable);
    ui->m_cbH21->setChecked(enable);
    ui->m_cbH31->setChecked(enable);
    ui->m_cbH41->setChecked(enable);
    ui->m_cbH51->setChecked(enable);
    ui->m_cbH61->setChecked(enable);
    ui->m_cbH71->setChecked(enable);
    ui->m_cbH81->setChecked(enable);
}
/** Des/habilita todas las row 1*/
void In2ConfigXUSB::sltEnableAllRow2(bool enable){
    ui->m_cbH12->setChecked(enable);
    ui->m_cbH22->setChecked(enable);
    ui->m_cbH32->setChecked(enable);
    ui->m_cbH42->setChecked(enable);
    ui->m_cbH52->setChecked(enable);
    ui->m_cbH62->setChecked(enable);
    ui->m_cbH72->setChecked(enable);
    ui->m_cbH82->setChecked(enable);
}

/** Testea los heads*/
void In2ConfigXUSB::sltTestHeads(){
    int card=m_card;
    char buffer[10];

    GestorImpresion *gestor=GestorImpresion::request(this);
    //Head 0
    QString desc=gestor->getHeadStatusDescription(card,0);
    ui->m_leState1->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    int serial=gestor->checkSerialHead(card,0);
    ui->m_leSerial1->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[0][0]) ui->m_cbH11->setChecked(true);
    else ui->m_cbH11->setChecked(false);
    if (m_parameters->m_bSelectHead[0][1]) ui->m_cbH12->setChecked(true);
    else ui->m_cbH12->setChecked(false);

    //Head 1
    desc=gestor->getHeadStatusDescription(card,1);
    ui->m_leState2->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    serial=gestor->checkSerialHead(card,1);
    ui->m_leSerial2->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[1][0]) ui->m_cbH21->setChecked(true);
    else ui->m_cbH21->setChecked(false);
    if (m_parameters->m_bSelectHead[1][1]) ui->m_cbH22->setChecked(true);
    else ui->m_cbH22->setChecked(false);

    //Head 2
    desc=gestor->getHeadStatusDescription(card,2);
    ui->m_leState3->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    serial=gestor->checkSerialHead(card,2);
    ui->m_leSerial3->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[2][0]) ui->m_cbH31->setChecked(true);
    else ui->m_cbH31->setChecked(false);
    if (m_parameters->m_bSelectHead[2][1]) ui->m_cbH32->setChecked(true);
    else ui->m_cbH32->setChecked(false);

    //Head 3
    desc=gestor->getHeadStatusDescription(card,3);
    ui->m_leState4->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    serial=gestor->checkSerialHead(card,3);
    ui->m_leSerial4->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[3][0]) ui->m_cbH41->setChecked(true);
    else ui->m_cbH41->setChecked(false);
    if (m_parameters->m_bSelectHead[3][1]) ui->m_cbH42->setChecked(true);
    else ui->m_cbH42->setChecked(false);

    //Head 4
    desc=gestor->getHeadStatusDescription(card,4);
    ui->m_leState5->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    serial=gestor->checkSerialHead(card,4);
    ui->m_leSerial5->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[4][0]) ui->m_cbH51->setChecked(true);
    else ui->m_cbH51->setChecked(false);
    if (m_parameters->m_bSelectHead[4][1]) ui->m_cbH52->setChecked(true);
    else ui->m_cbH52->setChecked(false);

    //Head 5
    desc=gestor->getHeadStatusDescription(card,5);
    ui->m_leState6->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    serial=gestor->checkSerialHead(card,5);
    ui->m_leSerial6->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[5][0]) ui->m_cbH61->setChecked(true);
    else ui->m_cbH61->setChecked(false);
    if (m_parameters->m_bSelectHead[5][1]) ui->m_cbH62->setChecked(true);
    else ui->m_cbH62->setChecked(false);

    //Head 6
    desc=gestor->getHeadStatusDescription(card,6);
    ui->m_leState7->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    serial=gestor->checkSerialHead(card,6);
    ui->m_leSerial7->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[6][0]) ui->m_cbH71->setChecked(true);
    else ui->m_cbH71->setChecked(false);
    if (m_parameters->m_bSelectHead[6][1]) ui->m_cbH72->setChecked(true);
    else ui->m_cbH72->setChecked(false);

    //Head 7
    desc=gestor->getHeadStatusDescription(card,7);
    ui->m_leState8->setText(desc);
    memset(&buffer,0x0,sizeof(buffer));
    serial=gestor->checkSerialHead(card,7);
    ui->m_leSerial8->setText(QString("%1").arg(serial));
    if (m_parameters->m_bSelectHead[7][0]) ui->m_cbH81->setChecked(true);
    else ui->m_cbH81->setChecked(false);
    if (m_parameters->m_bSelectHead[7][1]) ui->m_cbH82->setChecked(true);
    else ui->m_cbH82->setChecked(false);


}

/** Guarda datos*/
void In2ConfigXUSB::sltSaveData(){

    In2XmlParser parser(0,m_strConfigFile);
    (ui->m_cbH11->isChecked()? m_parameters->m_bSelectHead[0][0]=1:m_parameters->m_bSelectHead[0][0]=0);
    (ui->m_cbH12->isChecked()? m_parameters->m_bSelectHead[0][1]=1:m_parameters->m_bSelectHead[0][1]=0);
    (ui->m_cbH21->isChecked()? m_parameters->m_bSelectHead[1][0]=1:m_parameters->m_bSelectHead[1][0]=0);
    (ui->m_cbH22->isChecked()? m_parameters->m_bSelectHead[1][1]=1:m_parameters->m_bSelectHead[1][1]=0);
    (ui->m_cbH31->isChecked()? m_parameters->m_bSelectHead[2][0]=1:m_parameters->m_bSelectHead[2][0]=0);
    (ui->m_cbH32->isChecked()? m_parameters->m_bSelectHead[2][1]=1:m_parameters->m_bSelectHead[2][1]=0);
    (ui->m_cbH41->isChecked()? m_parameters->m_bSelectHead[3][0]=1:m_parameters->m_bSelectHead[3][0]=0);
    (ui->m_cbH42->isChecked()? m_parameters->m_bSelectHead[3][1]=1:m_parameters->m_bSelectHead[3][1]=0);
    (ui->m_cbH51->isChecked()? m_parameters->m_bSelectHead[4][0]=1:m_parameters->m_bSelectHead[4][0]=0);
    (ui->m_cbH52->isChecked()? m_parameters->m_bSelectHead[4][1]=1:m_parameters->m_bSelectHead[4][1]=0);
    (ui->m_cbH61->isChecked()? m_parameters->m_bSelectHead[5][0]=1:m_parameters->m_bSelectHead[5][0]=0);
    (ui->m_cbH62->isChecked()? m_parameters->m_bSelectHead[5][1]=1:m_parameters->m_bSelectHead[5][1]=0);
    (ui->m_cbH71->isChecked()? m_parameters->m_bSelectHead[6][0]=1:m_parameters->m_bSelectHead[6][0]=0);
    (ui->m_cbH72->isChecked()? m_parameters->m_bSelectHead[6][1]=1:m_parameters->m_bSelectHead[6][1]=0);
    (ui->m_cbH81->isChecked()? m_parameters->m_bSelectHead[7][0]=1:m_parameters->m_bSelectHead[7][0]=0);
    (ui->m_cbH82->isChecked()? m_parameters->m_bSelectHead[7][1]=1:m_parameters->m_bSelectHead[7][1]=0);


    QStringList params=QStringList()<<"XUSB"<<"Configuration"<<"PrintMode";
    QString val=QString("%1").arg(ui->m_cbImpresion->currentIndex());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar Tipo de Impresion"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"Common"<<"CopyCount";
    val=QString("%1").arg(ui->m_leCopias->text());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar Numero de copias"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"Common"<<"EncoderResolution";
    val=QString("%1").arg(ui->m_leEncoderRes->text());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar EncoderResolution"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"Common"<<"RequiredDPI";
    val=QString("%1").arg(ui->m_leRequiredDPI->text());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar RequiredDPI"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"Common"<<"TransportSpeed";
    val=QString("%1").arg(ui->m_leTransportSpeed->text());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar TransportSpeed"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"HeadOffsetRegistersCard1"<<"GlobalOffset";
    val=QString("%1").arg(ui->m_leGOffsetpuntos->text());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar GlobalOffset"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"Configuration"<<"SequenceStart";
    val=QString("%1").arg(ui->m_cbTipoPD->currentIndex());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar TipoPD"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"Configuration"<<"SysClock";
    val=QString("%1").arg(ui->m_cbTipoEncoder->currentIndex());
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar TipoEncoder"),QMessageBox::Ok);
    }
    params=QStringList()<<"XUSB"<<"Configuration"<<"DDFSValue";
    float x=ui->m_leFreqEncoderInterno->text().toFloat()/FACTOR_ENCODER;
    val=QString("%1").arg(x);
    if (!parser.setValue(params,"value",val)){
        QMessageBox::warning(0,"Error",tr("No se pudo guardar TipoEncoder"),QMessageBox::Ok);
    }

    parser.saveContentsToFile(m_strConfigFile);
    this->deleteLater();
}
