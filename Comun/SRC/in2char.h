#ifndef IN2CHAR_H
#define IN2CHAR_H

#include <QObject>

class In2Char : public QObject
{
    Q_OBJECT
public:
    explicit In2Char(QObject *parent = 0);
    char fromAscii(char x);
    char fromHex(char x);
    
signals:
    
public slots:
    
};

#endif // IN2CHAR_H
