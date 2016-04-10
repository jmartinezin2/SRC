#ifndef NUMERICPAD_H
#define NUMERICPAD_H

#include <QWidget>

namespace Ui {
class NumericPad;
}

class NumericPad : public QWidget
{
    Q_OBJECT
    
public:
    static NumericPad *request(QWidget *parent);
    static bool exist();

private:
    NumericPad(QWidget *parent = 0);
    ~NumericPad();

private slots:
    void sltAdd0();
    void sltAdd1();
    void sltAdd2();
    void sltAdd3();
    void sltAdd4();
    void sltAdd5();
    void sltAdd6();
    void sltAdd7();
    void sltAdd8();
    void sltAdd9();
    void sltAddPunto();
    void sltOK();
    void sltBack();

private:
    void addChar(QString data);
private:
    Ui::NumericPad *ui;
    static NumericPad *mInstance;

signals:
    //Señal de que se actualicen parametros
    void Update(const QString  & value);
};

#endif // NUMERICPAD_H
