#include "reportgeneratorui.h"
#include "ui_reportgeneratorui.h"

ReportGeneratorUI::ReportGeneratorUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportGeneratorUI)
{
    ui->setupUi(this);
}

ReportGeneratorUI::~ReportGeneratorUI()
{
    delete ui;
}
