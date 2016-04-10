/** @addtogroup ParImpresion
 *  @{*/
/** @file NivelTintas.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: NivelTintas.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef NIVELTINTAS_H
#define NIVELTINTAS_H

#include "inkjetform.h"
#include <QComboBox>
#include <QTextEdit>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include "ui_FluidicaUI.h"
#include "inkjetcommon.h"

class NivelTintas : public QWidget
{
	Q_OBJECT

public:
	//Constructor
	NivelTintas(QWidget *parent);
	//Destructor
	~NivelTintas();

private slots:
	//Acepta la seleccion de parametros
	void AceptarSeleccion();

    void UpdateNumeroEquipo();
    void UpdateUsarEquipo();

	void TestRoot();

	void ActualizaDataSlot();

	void ShowEstado();
	void ShowPHInfo(int index);

	void RebootHydra();
	void StartHydra();
	void StopHydra();
	void ShutdownHydra();
    void SetDataCommon();
    void PausarEquipo();

public slots:


private:
    Ui::FluidicaUI *ui;
    InkjetCommon *DataCommon;

signals:
    void SendDebugInfo(unsigned char code, QString msg);

};

#endif // NIVELTINTAS_H
