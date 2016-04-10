#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MemoryMap.h"

namespace Ui {
class MainWindow;
}

class Principal : public QObject/*public QMainWindow*/
{
    Q_OBJECT

public:
    explicit Principal(QObject *parent = 0);
    ~Principal();

public slots:
    void ReceiveDebugInfo(unsigned char codigo, QString mensaje);

private:
    Ui::MainWindow *ui;
    MemoryMap *m_memoryMap;
};

#endif // MAINWINDOW_H
