/** @addtogroup Usuarios
 *  @{*/
/** @file Usuarios.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo de configuracion de la aplicacion inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Usuarios.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef USUARIOS_H
#define USUARIOS_H

#include "inkjetform.h"
#include "globals.h"
#include "ui_usuariosUI.h"
#include "inkjetcommon.h"
#include <QLabel>
#include <QPixmap>


class Usuarios : /*public InkjetForm*/ public QWidget
{
	Q_OBJECT

public:
	Usuarios(QWidget *parent);
	~Usuarios();
	//Unico mapa accesible
	//Usuario MapaUsuarios[NUM_USUARIOS];
	char PasswordUsuario[16];

private slots:
	//Lanza la pantalla principal
	void Atras();
	void InputPassword();
	void LaunchPrincipal();	
    void AdministrarUsuarios();
    void UsuarioIntroducido();

	//Actualiza la info
    //void ActualizaDataSlot();
    //void ActualizaDataSlotOPC();

	//Testea el password
    //void CheckRootPassword(QString);
	void TestRoot();
	int LoadUsersMap();
	int SaveUsersMap();
    //void setDataCommon();
    void MuestraImagenPrincipal();

private:
    Ui::UsuariosUI *ui;
    MemoryMap *m_memoryMap;
};


#endif // USUARIOS_H
