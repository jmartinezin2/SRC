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
#include "gestorcontrol.h"
#include "gestorcontrol.h"
#include "globals.h"


GestorControl::GestorControl(QObject *parent) :
    QObject(parent)
{
    m_ControlSocket=new QUdpSocket(this);
    if (!m_ControlSocket->bind(QHostAddress::LocalHost, DEFAULT_CONTROL_PORT)){

    }
    else{
        connect(m_ControlSocket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
    }

}

/** Leemos lo que llega*/
void GestorControl::readPendingDatagrams(){
    while (m_ControlSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_ControlSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        m_ControlSocket->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        processDatagram(datagram,sender,senderPort);
        //De momento sólo lo rebotamos
        //m_ControlSocket->writeDatagram(datagram,sender,senderPort);
    }
}
/** Mandamos datagrama*/
void GestorControl::sendDatagram(EthMessage *p,const QHostAddress & sender, quint16 senderPort){
    m_ControlSocket->writeDatagram(p->message(),sender,senderPort);
}

/** Procesamos datagrama*/
void GestorControl::processDatagram(const QByteArray &datagram,const QHostAddress & sender, quint16 senderPort){
    EthMessage *msg=new EthMessage(0,datagram);
    if (msg->isValidMessage()){
        char cType=msg->getID();
        switch (cType){
            case UDP_IS_ALIVE:
                msg->SetBitACK();
                m_ControlSocket->writeDatagram(msg->message(),sender,senderPort);
            break;
            case UDP_REQ_STATE:
            break;
            case UDP_GET_LASTERROR:
            break;
            case UDP_GET_USERINFO:
            break;
            case UDP_GET_CURRENTFILE:
            break;
            case UDP_GET_MEMORYMAP:
            break;
            case UDP_SENDPD:
                //Esto ya lo sacaremos fuera
                //SendDebugInfo(LOGGER_WARNING,"UDP rcv: PD");
                //bXaarScorpionGenerateProductDetect(0);
                msg->SetBitACK();
                m_ControlSocket->writeDatagram(msg->message(),sender,senderPort);
            break;
            case UDP_SETPD:
                //Esto ya lo sacaremos fuera
                //bXaarScorpionSetPDInternal(0,msg->message().at(DATA_POS));
                msg->SetBitACK();
                m_ControlSocket->writeDatagram(msg->message(),sender,senderPort);
            break;
            //No sabemos lo que es
            default:
                msg->SetUnknownMessage();
                m_ControlSocket->writeDatagram(msg->message(),sender,senderPort);
            break;
        }
    }
}
