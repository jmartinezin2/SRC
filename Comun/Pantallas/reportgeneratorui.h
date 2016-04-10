#ifndef REPORTGENERATORUI_H
#define REPORTGENERATORUI_H

#include <QWidget>

namespace Ui {
class ReportGeneratorUI;
}

class ReportGeneratorUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit ReportGeneratorUI(QWidget *parent = 0);
    ~ReportGeneratorUI();
    
private:
    Ui::ReportGeneratorUI *ui;
};

#endif // REPORTGENERATORUI_H
