/** @addtogroup ConfUsers
 *  @{*/
/** @file ConfUsers.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de configuracion de tintas
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: ConfUsers.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#ifndef CONFUSERS_H
#define CONFUSERS_H

#include <QObject>
#include "inkjetform.h"
#include "inputdata.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include "ui_ConfUsersUI.h"
#include "inkjetcommon.h"

class ConfUsers : public QWidget
{
	Q_OBJECT

public:
	ConfUsers(QWidget *parent);
	~ConfUsers();

private:
	//Actualiza tamaño de la tabla
	void AjustaTabla();
	//Actualiza un campo de la tabla
	void ActualizaCampo(QString tmp);
    //Inicializa parametros comunes
    //void setDataCommon();

private slots:

	void Atras();
	//Nuevo usuario
    void UsuarioNuevo();
	//Eliminar usuario
	void UsuarioEliminar();
	void UsuarioEliminar_SinPreguntar();
	//Modificar usuario
	void UsuarioModificar();
	//Se ha seleccionado un campo de la tabla
	void SeleccionCampo(int x, int y);
	//Actualiza la informacion de pantalla
	void UpdateInfo();
	//Actualiza la info
    //void ActualizaDataSlot();
	//Test root
	void TestRoot();

	void UpdateNombreUsuario();
    //void UpdateTipoUsuario(QString seleccion);

    void UpdatePasswordUsuario();
    void UpdatePassword(QString seleccion);

	void UpdateNombre(QString value);

	void AceptarValores();
	void CancelarValores();
	void InsertarValores();

	void CargarValoresTipo();
    int TestPasswordStrength();
	
public slots:	
	void InputTeclado(QString Buffer);
	
private:
    Ui::ConfUsersUI *ui;
    MemoryMap *m_memoryMap;
    QStringList TipoUsuarios;
    unsigned char NumeroTipoUsuarios;
    QStringList TipoUsuariosSeleccionables;
    unsigned char NumeroTipoUsuariosSeleccionables;

    //InkjetCommon *DataCommon;

};

#endif // CONFUSERS_H
