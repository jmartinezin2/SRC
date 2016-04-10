#ifndef IN2GHOSTSCRIPTCONFIG_H
#define IN2GHOSTSCRIPTCONFIG_H

#include <QDialog>

namespace Ui {
class In2GhostScriptConfig;
}

class In2GhostScriptConfig : public QDialog
{
    Q_OBJECT
    
public:
    explicit In2GhostScriptConfig(QWidget *parent = 0);
    ~In2GhostScriptConfig();

private slots:
    void sltSaveData();
    void sltChangeICCFile();
    void sltChangePath();
    
private:
    Ui::In2GhostScriptConfig *ui;
};

#endif // IN2GHOSTSCRIPTCONFIG_H
