#ifndef SERIALPORT_H
#define SERIALPORT_H

//Buffer de transmision
#define MAX_SIZE_SMS 32
#include "qextserialport.h"


class SerialPort : public QextSerialPort
{
	Q_OBJECT

public:
	SerialPort();
	SerialPort(const QString & name);
	SerialPort(const PortSettings& settings);
	SerialPort(const QString & name, const PortSettings& settings);
	~SerialPort();
	int Read(char *Buffer,qint64 size);
	void SendMessage(int idmsg);
	qint64 SendData(char *data,qint64 size);
	void SendActualizaData(void);
	unsigned char CalculoCheckSum_TX(unsigned char NumBytes);
	unsigned char CalculoCheckSum(char *Buffer,unsigned char NumBytes);

signals:
	//Señal de que se ha procesado 1 sms-index del microprocesador
	void TramaMicroprocesador();
	void Completado(int idone);
	void ForzarEnvio();
	void ForzarEnvioImagenFija();

private:
	//Analiza trama del microprocesador
	void AnalizaTramaMaquina(char *Buffer,quint64 size);
private:
	int iBytesImprimir;
public:
	unsigned char iFlag;
};

#endif // SERIALPORT_H
