
#include "serialport.h"
#include "globals.h"
#include "principal.h"
#include "string.h"
#include "comthread.h"
#include <QMessageBox>

QByteArray BufferTx;

SerialPort::SerialPort():QextSerialPort() {
}
SerialPort::SerialPort(const QString & name):QextSerialPort(name) {
}
SerialPort::SerialPort(const PortSettings& settings):QextSerialPort(settings) {
}
SerialPort::SerialPort(const QString & name, const PortSettings& settings):QextSerialPort(name, settings) {
}
SerialPort::~SerialPort(){
}



/* Lee del puerto serie*/
int SerialPort::Read(char *Buffer,qint64 size){

	char tmp[32];
	static int iRcv=0;
	int iNumBytes=0;
	iNumBytes=this->bytesAvailable();

	if (iNumBytes>0){
		iNumBytes=this->read(tmp,iNumBytes);
		//Es una trama del microprocesador...me sincronizo con ella
                if ((unsigned char)tmp[0]==0xAA){
                    //Trama de datos de micro
                    iRcv=0;
                        memcpy(Buffer+iRcv,tmp,iNumBytes);
                        iRcv=iRcv+iNumBytes;
                        if (iRcv>=3) {
                            AnalizaTramaMaquina(Buffer,size);
                            iRcv=0;
                        }
			return 1;
		}
		else{
			return -1;
		}
	}
	else return 1;

}


/* Modulo de mensajes serie*/

//Analiza la trama que viene del microprocesador
void SerialPort::AnalizaTramaMaquina(char *Buffer, quint64 size){


}

void SerialPort::SendMessage(int idmsg){


}


void SerialPort::SendActualizaData(){
//	emit ActualizaData();
}

qint64 SerialPort::SendData(char *data,qint64 size){

	this->writeData((char*)data,size);
	return size;
}

unsigned char SerialPort::CalculoCheckSum_TX(unsigned char NumBytes){

    unsigned char suma=0;

	for(int i=0;i<(NumBytes-1);i++) suma+=BufferTx[i];

	return suma;
	
}


unsigned char SerialPort::CalculoCheckSum(char *Buffer, unsigned char NumBytes){

    unsigned char suma=0;

    for(int i=0;i<(NumBytes-1);i++) suma+=Buffer[i];

	return suma;
	
}
