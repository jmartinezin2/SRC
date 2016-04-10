#ifndef CONFIGTABLAS_H
#define CONFIGTABLAS_H

#include <QWidget>
#include <QGraphicsScene>
#include "ui_configtablasUI.h"
#include "globals.h"
#include "inkjetcommon.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

/*namespace Ui {
class ConfigTablasUI;
}*/

class ConfigTablas : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConfigTablas(QWidget *parent = 0);
    ~ConfigTablas();
private slots:
    void sltActualiza(int x,int y);
    void sltCellSelected(int x,int y);
    void Salir();
    void setDataCommon();
    void UpdateGrafico();
    void UpdateInfo();
    void UpdateCurva();
    void ActualizaCampo(QString item);
    void RegistroNuevo();
    void RegistroEliminar();
    void RegistroModificar();
    void InsertarValores();
    void CancelarValores();
    void Eliminar_SinPreguntar();
    void UpdateXPunto();
    void UpdateX(QString value);
    void UpdateYPunto();
    void UpdateY(QString value);
    void CambioTipoCurva();
private:
    Ui::ConfigTablasUI *ui;
    //Estructura comun
    InkjetCommon *dataCommon;

    int iFila;
    int iColumna;
    int TipoGrafico;

    QStringList NombreColumnasTablaRefVelocidadIntensidad;
    unsigned char NumeroColumnasTablaRefVelocidadIntensidad;

    QStringList NombreColumnasTablaRefVelocidadIntensidad2C;
    unsigned char NumeroColumnasTablaRefVelocidadIntensidad2C;

    QStringList NombreColumnas;
    unsigned char NumeroColumnas;

    unsigned char flag_modificando_xy;
    QString tmpx;
    QString tmpy;

    QwtPlotGrid *grid;
    QwtPlotCurve *curve;
    QwtSymbol *symbol;

signals:
    void SendDebugInfo(unsigned char code, QString msg);

};

#endif // CONFIGTABLAS_H
