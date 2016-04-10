#ifndef LECMESSAGE_H
#define LECMESSAGE_H

#include <QByteArray>

class LecMessage
{
public:
    LecMessage();
    QByteArray CreateMessageWithData(QByteArray &d);
    unsigned char calculateLRC(QByteArray &trama);

private:

private:
    QByteArray m_data;
    char m_lastError;

};

#endif // LECMESSAGE_H
