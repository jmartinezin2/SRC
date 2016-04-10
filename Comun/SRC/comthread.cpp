
/** @addtogroup ComThread
 *  @{*/
/** @file ComThread.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Hilo de comunicaciones con el microprocesador
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ComThread.cpp
 *    @li Versión: x.0
 *    @li Fecha: 26/08/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#include "comthread.h"
#include "principal.h"
#include "globals.h"


QWidget *myparent;



ComThread::ComThread(unsigned char index,QWidget *parent,BaudRateType ibaudios)
	: QThread()
{
	iRun=0;
	iPuerto=index;
	myparent=parent;
	//status=new Status(parent);
	SerialMicro=NULL;
	ibaud=ibaudios;
	
}

ComThread::~ComThread()
{
	if (SerialMicro) delete (SerialMicro);
	this->deleteLater();
}

void ComThread::run(){

	char temp[6];
	
    #ifdef __WINDOWS__
       _snprintf(temp,sizeof(temp),"COM%d",iPuerto);
    #else
        _snprintf(temp,sizeof(temp),"/dev/ttyS%d",iPuerto-1);
    #endif
        //snprintf(temp,sizeof(temp),"/dev/ttyS%d",iPuerto-1);

	SerialMicro=new SerialPort(temp);

	if (SerialMicro){
		SerialMicro->setBaudRate(ibaud);
		SerialMicro->setFlowControl(FLOW_OFF);
		SerialMicro->setParity(PAR_NONE);
		SerialMicro->setDataBits(DATA_8);
		SerialMicro->setStopBits(STOP_1);

		if (SerialMicro->open(QIODevice::ReadWrite)){
		//A leer
			connect(SerialMicro, SIGNAL(readyRead()), this, SLOT(receiveMsg()));
			iRun=1;
		}
		else iRun=-1;
	}
	else iRun=-1;
	
	while (1){
		if (iRun==1) SerialMicro->Read(Buffer,sizeof(Buffer));
		usleep(1);
	}
	this->close();
	iRun=-1;
}
int ComThread::receiveMsg(){
	return SerialMicro->Read(Buffer,sizeof(Buffer));
}
void ComThread::sendMessage(int index){
	SerialMicro->SendMessage(index);
}
void ComThread::close(){
	if (SerialMicro->isOpen()){
		SerialMicro->close();
		iRun=0;
	}
}
signed char ComThread::isRunning(){
	return iRun;
}
qint64 ComThread::writeData(char *data,qint64 size){
	
	SerialMicro->SendData(data,size);
	SerialMicro->flush();
	
	return size;
}
//Reconecta el puerto serie donde digamos
int ComThread::Reconectar(unsigned char index,BaudRateType ibaud){
	
	char temp[6];

    //snprintf(temp,sizeof(temp),"COM%d",index);
    //snprintf(temp,sizeof(temp),"/dev/ttyS%d",iPuerto-1);

	if (iRun==1) SerialMicro->close();
	iRun=0;
	if (SerialMicro){
		SerialMicro->setPortName(temp);
	}
	else{
		SerialMicro=new SerialPort(temp);
	}
	if (SerialMicro){
		SerialMicro->setBaudRate(ibaud);   
		SerialMicro->setFlowControl(FLOW_OFF);
		SerialMicro->setParity(PAR_NONE);    
		SerialMicro->setDataBits(DATA_8);   
		SerialMicro->setStopBits(STOP_1);
		if (SerialMicro->open(QIODevice::ReadWrite)){
		//A leer
			connect(SerialMicro, SIGNAL(readyRead()), this, SLOT(receiveMsg()));
			iRun=1;
		}
		else iRun=-1;
	}
	else iRun=-1;

	return iRun;
}
