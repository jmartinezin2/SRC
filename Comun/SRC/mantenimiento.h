/** @addtogroup mantenimiento
 *  @{*/
/** @file mantenimiento.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de mantenimiento
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: mantenimiento.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#ifndef MANTENIMIENTO_H
#define MANTENIMIENTO_H

#include <QObject>
#include "inkjetform.h"
#include "inputdata.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include "ui_mantenimientoUI.h"
#include "inkjetcommon.h"
#include "MemoryMap.h"

class mantenimiento : public QWidget
{
	Q_OBJECT

public:
    mantenimiento(QWidget *parent);
    ~mantenimiento();

private:
	//Actualiza tamaño de la tabla
	void AjustaTabla();
	//Actualiza un campo de la tabla
	void ActualizaCampo(QString tmp);
    //Inicializa parametros comunes
    //void setDataCommon();

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

    void sltLeerBBDD(void);

    void ActualizarDiaLimpiezaLampara();
	
public slots:	
	void InputTeclado(QString Buffer);
	
private:
    Ui::mantenimientoUI *ui;
    MemoryMap *m_memoryMap;
    //InkjetCommon *DataCommon;

};

#endif // MANTENIMIENTO_H
