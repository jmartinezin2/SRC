/** @addtogroup ComThread
 *  @{*/
/** @file ComThread.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
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

#ifndef COMTHREAD_H
#define COMTHREAD_H

#include <QThread>
#include <QTimer>
#include "serialport.h"

//Tamaño del buffer de transmisión y de recepción

#define MAX_SIZE_MICRO_RX 32	//2907200 Antes estaba a 128
#define MAX_SIZE_MICRO_TX 32	//2907200 Antes estaba a 128

class ComThread : public QThread
{
	Q_OBJECT
public:
	ComThread(unsigned char index,QWidget *parent,BaudRateType ibaud);
	~ComThread();
	void run();
	void close();
	signed char isRunning();
	qint64 writeData(char *data,qint64 size);
	void sendMessage(int index);
	int Reconectar(unsigned char index,BaudRateType ibaud);
	
public slots:
	int receiveMsg();
private slots:

protected:
        //Buffer de Rx
        char Buffer[MAX_SIZE_MICRO_RX];
	//Buffer de Tx
	char BufferTx[MAX_SIZE_MICRO_TX];
	unsigned char iPuerto;
	signed char iRun;
	BaudRateType ibaud;
	//int ibaud;
private:
public:
	//Status *status;
//Puerto serie asociado
	SerialPort *SerialMicro;
//Porcentaje de impresion completado
	int iCompletado;

};

#endif // COMTHREAD_H
