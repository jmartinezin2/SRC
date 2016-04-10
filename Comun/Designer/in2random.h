#ifndef IN2RANDOM_H
#define IN2RANDOM_H

#include <QObject>

class In2Random : public QObject
{
    Q_OBJECT
public:
    explicit In2Random(QObject *parent = 0);
    ~In2Random();
    int generarNumeroAleatorio(int limiteInferior,int limiteSuperior);
    void cambiarSemillaNumeroAleatorio(void);


signals:

public slots:
};

#endif // IN2RANDOM_H
