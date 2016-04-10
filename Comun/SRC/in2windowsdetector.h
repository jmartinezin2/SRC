#ifndef IN2WINDOWSDETECTOR_H
#define IN2WINDOWSDETECTOR_H

#include <QObject>
#include <windows.h>

class In2WindowsDetector : public QObject
{
    Q_OBJECT
public:
    explicit In2WindowsDetector(QObject *parent = 0);
    HWND detectWindow(QString txt);
    void showWindow(HWND handler);
signals:
    
public slots:
    
};

#endif // IN2WINDOWSDETECTOR_H
