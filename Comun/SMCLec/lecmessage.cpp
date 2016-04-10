#include "lecmessage.h"
#include "in2char.h"

LecMessage::LecMessage()
{
}


/** Crea mensaje con datos*/
QByteArray LecMessage::CreateMessageWithData(QByteArray &d){
    m_data.clear();
    m_data.append(d);
    In2Char z;
    unsigned char dummy=calculateLRC(d);
    unsigned char clrc1=(dummy&0xF0)>>4;
    unsigned char clrc2=(dummy&0x0F);
    unsigned char lrc1=z.fromHex(clrc1);
    unsigned char lrc2=z.fromHex(clrc2);
    m_data.append(lrc1);
    m_data.append(lrc2);
    return m_data;
}


unsigned char LecMessage::calculateLRC(QByteArray &trama){
    unsigned short suma=0xFFFF;
    unsigned short sumando=0xFFFF;
    char dummy;
    bool ok=false;
    unsigned char uSuma,uSuma2;
    for (int i=0;i<trama.size();i++){
        dummy=trama.at(i);
        sumando=QString("%1").arg(dummy).toUtf8().toInt(&ok,16);
        suma^=sumando;
        suma=(summa>>1);
    }
    uSuma=(unsigned char) (suma&0xFF);
    uSuma2=(unsigned char) ~uSuma;
    uSuma=uSuma2+1;
    return (uSuma);

}
