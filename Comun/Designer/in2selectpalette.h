#ifndef IN2SELECTPALETTE_H
#define IN2SELECTPALETTE_H

#include <QWidget>
#include "ui_In2SelectPalette.h"
#include <QCheckBox>

class In2SelectPalette : public QWidget
{
    Q_OBJECT
public:
    explicit In2SelectPalette(QWidget *parent = 0,QString id=0);
    void setPalette(QVector<QRgb> &pal);
    void comparePalette(QVector<QRgb> &pal);

signals:
    //Cadena de rgbs separados por @
    void sgnNewColor(QString &color,QString &id);

private slots:
    void sltSelectAll();
    void sltDeSelectAll();
    void sltCancel();
    void sltExit();
    void sltAcceptAndExit();
    void sltUpdateCounter(bool checked);

public slots:

private:
    Ui::In2SelectPalette *ui;
    QVector<QCheckBox *> m_cb;
    QVector<QRgb> m_pal;
    QString m_id;
};

#endif // IN2SELECTPALETTE_H
