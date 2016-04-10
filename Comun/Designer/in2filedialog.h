#ifndef IN2FILEDIALOG_H
#define IN2FILEDIALOG_H

#include <QFileDialog>
#include <QSettings>
#include "globals.h"

class In2FileDialog : public QFileDialog
{
    Q_OBJECT
public:
    explicit In2FileDialog(QWidget *parent = 0);
    void saveSettings();
    void restoreSettings();
    
signals:
    
public slots:
    
};

#endif // IN2FILEDIALOG_H
