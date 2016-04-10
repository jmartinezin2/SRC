/**
 * @file puertoSerie.h
 * @brief Port Listener.
 * @see puertoSerie
 */

#ifndef PUERTOSERIE_H_
#define PUERTOSERIE_H_

#include <QObject>
#include <QLabel>

class QextSerialPort;

class puertoSerie : public QObject
{
	Q_OBJECT
	
	public:
        static puertoSerie *request(QextSerialPort *port, QObject *parent);
        puertoSerie(QextSerialPort * port, QObject * parent = 0);
        QextSerialPort * port;
        QByteArray BufferRx;
        QByteArray BufferTx;

    private:
        //puertoSerie(QObject *parent = 0);
        static puertoSerie *mInstance;
	
	public slots:
		void receive();
        QLabel *ShowMensaje(QString texto);

    signals:
        void mensajeModbusRecibido(QByteArray data);
        void mensajeModbusRecibido();

};


#endif /*PUERTOSERIE_H_*/
