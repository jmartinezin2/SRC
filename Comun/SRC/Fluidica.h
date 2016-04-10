/** @addtogroup Fluidica
 *  @{*/
/** @file Fluidica.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Fluidica.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef Fluidica_H
#define Fluidica_H

#include "inkjetform.h"
#include <QComboBox>
#include <QTextEdit>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include "ui_FluidicaUI.h"
#include "inkjetcommonext.h"
#include "in2gestorfluidica.h"

class Fluidica : public QWidget
{
	Q_OBJECT

public:
	//Constructor
	Fluidica(QWidget *parent);
	//Destructor
	~Fluidica();
protected:
    virtual void  closeEvent(QCloseEvent *event);

private slots:
	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	void TestRoot();
    void UpdateUsarEquipo(int equipo, bool usar);
    void RebootHydra_1();
    void StartHydra_1();
    void StopHydra_1();
    void ShutdownHydra_1();
    void RebootHydra_2();
    void StartHydra_2();
    void StopHydra_2();
    void ShutdownHydra_2();
    void RebootHydra_3();
    void StartHydra_3();
    void StopHydra_3();
    void ShutdownHydra_3();
    void RebootHydra_4();
    void StartHydra_4();
    void StopHydra_4();
    void ShutdownHydra_4();
    void RebootHydra_5();
    void StartHydra_5();
    void StopHydra_5();
    void ShutdownHydra_5();
    void sltActualizaDatos_1();
    void sltActualizaDatos_2();
    void sltActualizaDatos_3();
    void sltActualizaDatos_4();
    void sltActualizaDatos_5();
    void sltCheckClicked(int row, int col);
    void setColor(int equipo);
    void setDescripcion(QString txt, int equipo);
    void setTSuministro(QString txt, int equipo);
    void setTDeposito(QString txt, int equipo);
    void setTRetorno(QString txt, int equipo);
    void setPSuministro(QString txt, int equipo);
    void setPRetorno(QString txt, int equipo);
    void setPDiferencial(QString txt, int equipo);
    void setPMenisco(QString txt, int equipo);
    void setNDeposito(QString txt, int equipo);
    void setHydraLastError(QString txt, int equipo);
    void setState(int equipo);
    void setLEDs(int equipo);
    void setSwitches(int equipo);


private:
    void InitTabla();

public slots:


private:
    Ui::FluidicaUI *ui;
    InkjetCommonExt *DataCommon;
    In2GestorFluidica *m_gestor;
    float m_supplyOffset;
    float m_returnOffset;
    QWidget *m_parent;
    MemoryMap *m_memoryMap;

signals:
    void SendDebugInfo(unsigned char code, QString msg);

};

#endif // Fluidica_H
