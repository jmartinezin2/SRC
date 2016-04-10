#ifndef IN2DEBUGCLEANER_H
#define IN2DEBUGCLEANER_H

#include <QObject>
#include "in2defines.h"

class In2DebugCleaner : public QObject
{
    Q_OBJECT
public:
    explicit In2DebugCleaner(QObject *parent = 0);
    ~In2DebugCleaner();
    bool exists();

signals:
    void SendDebugInfo(unsigned char val, QString txt);
private:
    bool testCurrentMonth();

public slots:
    void start();
private:
    QString m_strExecutable;

    
};

#endif // IN2DEBUGCLEANER_H
