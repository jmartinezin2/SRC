/** @addtogroup Curvas
 *  @{*/
/** @file Curvas.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de Curvas
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Curvas.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#ifndef CURVAS_H
#define CURVAS_H

#include <QObject>
#include "inkjetform.h"
#include "inputdata.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include "ui_curvasUI.h"
#include "inkjetcommon.h"

class Curvas : public QWidget
{
	Q_OBJECT

public:
    Curvas(QWidget *parent);
    ~Curvas();

private:
	//Actualiza tamaño de la tabla
	void AjustaTabla();
	//Actualiza un campo de la tabla
	void ActualizaCampo(QString tmp);
    //Inicializa parametros comunes
    void setDataCommon();

private slots:

	void Atras();
    //Nuevo registro
    void RegistroNuevo();
    //Eliminar registro
    void RegistroEliminar();
    void RegistroEliminar_SinPreguntar();
    void RegistroEliminarTodo();
    //Modificar registro
    void RegistroModificar();
	//Se ha seleccionado un campo de la tabla
	void SeleccionCampo(int x, int y);
	//Actualiza la informacion de pantalla
	void UpdateInfo();
	//Actualiza la info
	void ActualizaDataSlot();
	//Test root
	void TestRoot();

    void UpdateTiempo(QString value);
    void UpdateTiempoRegistro();

    void UpdateFechaRegistro();
    void UpdateFecha(QString seleccion);

	void AceptarValores();
	void CancelarValores();
	void InsertarValores();

	void CargarValoresTipo();

    void ActualizarTiemposFuncionamiento(void);

    void ActualizarDiaLimpiezaLampara();
	
public slots:	
	void InputTeclado(QString Buffer);
	
private:
    Ui::curvasUI *ui;
    InkjetCommon *DataCommon;
    MemoryMap *m_memoryMap;
};

#endif // CURVAS_H
