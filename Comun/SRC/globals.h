/** @addtogroup Globals
 *  @{*/
/** @file Globlas.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions
 *
 *    @brief Fichero de variables globales
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: globals.h
 *    @li Versión: x.0
 *    @li Fecha: 30/04/13
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

//#include "XCL_Library.h"
//#include "ScorpionDLL-ext.h"
//#include "MemoryMap.h"
//#include "GestorDB_ODBC.h"
#include "GestorDB_SQLITE.h"
#include <QPixmap>
//#include <QAxObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <windows.h>
#include <QFont>
#include <QLabel>
#include <QDebug>
#include "qzint.h"



//Para los carteles auxiliares
#define AUX_INFO_WIDTH 200
#define AUX_INFO_HEIGHT 75

#define AUX_INFO_X  (1024-AUX_INFO_WIDTH)/2
#define AUX_INFO_Y  (768-AUX_INFO_HEIGHT)/2


//#define WDG_FILE "StartWDG.bat"
#define WDG_FILE "D:\\Data\\INKJET\\Xaar\\Installer\\InterfazWdg.exe"
//Aplicacion de calculo de tinta
#define INKCALCULATOR_FILE "inkCalculator.exe"
//Fichero temporal de calculo de tinta
#define TMP_INKCALCFILE "tmpInkCalc.bmp"
//Path de auditorias
#define DEFAULT_INKJETDEBUG_PATH "\\Output"
#define DEFAULT_INKJETDEBUG_FILE DEFAULT_INKJETDEBUG_PATH"\\debugFile"

//Fichero de configuración standard
#define CONFIG_FILE_NORMALSPEED "c:/xaar/XUSB.xml"
//Fichero de configuración de alta velocidad
#define CONFIG_FILE_HIGHSPEED "c:/xaar/XUSBHighSpeed.xml"
//Path de configuracion de XAAR
#define DEFAULT_XAAR_PATH "C:\\xaar"


//Tiempo de testeo por cabezal
//#define TIME_HEAD_TEST 2500


//Tamaños de letras
#define QFONT6	6
#define QFONT7	7
#define QFONT8	8
#define QFONT9	9
#define QFONT10	10
#define QFONT11	11
#define QFONT12	12

#define BUTTONW	110*FACTOR
#define BUTTONH  90*FACTOR_Y



//Posibles origenes de datos
enum{
    ORG_COMPOFILE=0, //Los datos viene según estén configurados en el fichero de conposicion
    ORG_DATABASE=1,  //Los datos viene en la BBDD
    ORG_EXTERNAL    //Los datos vienen en ficheros externos
};

//IDs de emergencias

/*#define EMERGENCIA_SETA                 0x01
#define EMERGENCIA_VARSALIDA            0x02
#define EMERGENCIA_VARENTRADA           0x04
#define EMERGENCIA_ARRASTRE             0x08
#define EMERGENCIA_BANDEJA              0x10
#define EMERGENCIA_MESA                 0x20
#define EMERGENCIA_CAPOTA               0x40
#define EMERGENCIA_PRESION              0x80
#define EMERGENCIA_LAMPARA_ELECTRICO    0x100
#define EMERGENCIA_LAMPARA_APAGADA     0x200*/

//Se definen leds aqui
#define LED_ROJO ":res/images/Ledrojo.png"
#define LED_VERDE ":res/images/Ledverde.png"
#define LED_AMBAR ":res/images/Ledambar.png"


//#ifdef TFT_1024x600

    #define FACTOR		1//0.8
    #define FACTOR_Y	1//0.8

    #define IMG_CENTRAL_SIZE 250

    //Tamaño en pixeles de la aplicación
    #define APP_HEIGHT 600
    #define APP_WIDTH  1024
    //Dimensiones frame central
    #define IMG_X	5
    #define IMG_Y	85
    #define	IMG_HEIGHT	385
    #define IMG_WIDTH	804
    //Dimensiones frame superior
    #define INFO_X	5
    #define INFO_Y	5
    #define	INFO_HEIGHT	75
    #define INFO_WIDTH	804
    //Dimensiones frame inferior
    #define STATUS_X	5
    #define STATUS_Y	470
    #define	STATUS_HEIGHT	125
    #define STATUS_WIDTH	804
    //Dimensiones main
    #define MAIN_X	814
    #define MAIN_Y	5
    #define	MAIN_HEIGHT	590
    #define MAIN_WIDTH	205


    #define WICONSIZE	50
    #define	HICONSIZE	50
    //Proporciones de las tablas centrales
    #define WTABLA	500
    #define HTABLA	185

//#endif


#define IMAGEN_FV_SS		0	//Sin seleccionar
#define IMAGEN_FV_FIJA		1
#define IMAGEN_FV_VARIABLE	2

#define TIMEOUT 1000

#define ROTATE_W	256
#define ROTATE_H	256

#define	DELTA_X	150
#define	DELTA_Y	5
#define	DELTA_WIDTH	150
#define	DELTA_HEIGHT 20

#define IMG_OFFSET 20

//Definiciones de tamaños
#define TEXT_Y	150
#define	TEXT_W	400
#define TEXT_H	300
#define	DELTA_X	150

#define OPCIONES_FORMAT_WIDTH		150
#define OPCIONES_FORMAT_HEIGHT		367
#define IMG_OFFSET                  20

//JORGE
#define EDIT_W	200
#define	EDIT_H	28
#define DELTA	9

#define	FILE_X	25
#define	FILE_W	500
#define	FILE_H	15
#define	TEXT_X	15
#define	LABEL_OFFSET	18
#define LOGO_SIZE		75

//Dimensiones de pantalla
#define WTMP	250
#define	HTMP	35

#define	EDIT_H	28

//Tamaños
#define DELTA		9
#define OFFSET_X	25
#define OFFSET_Y	20

#define OPCIONES_FORMAT_WIDTH		150
#define OPCIONES_FORMAT_HEIGHT		367
#define IMG_OFFSET 20

//Tamaños
#define	WIDTH 600
#define	HEIGTH 300
#define MINHEIGHT	30
#define MINHEIGHT2	45

#define X0	100
#define Y0	90
#define XW	(APP_WIDTH-2*X0)
#define YH	(APP_HEIGHT-2*Y0)

#define	OFFSET_W	50
#define	OFFSET_H	50
#define	OFFSET2_X	160
#define	OFFSET2_Y	60

#define	OFFSET3_X	150
#define	OFFSET3_Y	15
#define	OFFSET2_W	175
#define	OFFSET2_H	200*

//Fcihero dll
#define XUSBDLL "c:\\xaar\\XUSBDLL.xml"
#define XUSBXML "c:\\xaar\\XUSB.xml"
#define XUSBHIGHSPEEDXML "c:\\xaar\\XUSBHighSpeed.xml"

//#define CURRENT_WEB_WIDTH   1000        //5 cabezales Xaar 1000

#define	TXT_ON		  "On"
#define	TXT_OFF		  "Off"

#define	TXT_IMAGEN_FONDO                "Imagen_fondo"
#define	TXT_NOMBRE_PDF                  "Nombre"
#define	TXT_RUTA_PDF                    "Ruta"
#define	TXT_X_INICIAL_IMAGEN_FONDO      "PuntoInicial_X"
#define	TXT_Y_INICIAL_IMAGEN_FONDO      "PuntoInicial_Y"
#define	TXT_ANCHO_IMAGEN_FONDO          "Ancho"
#define	TXT_ALTO_IMAGEN_FONDO           "Alto"

//Para saber en qué version de campos estamos
//VERSION_CAMPOS_1 = sólo una matriz por campo
//VERSION_CAMPOS_2 = varias matrices por campo
#define	VERSION_CAMPOS_1               1
#define	VERSION_CAMPOS_2               2


#define TXT_PROPIEDADES                 "PROPIEDADES"
#define TXT_CREADOPOR                   "CreadoPor"
#define TXT_CREADOCUANDO                "CreadoCuando"
#define TXT_MODIFICADOPOR               "ModificadoPor"
#define TXT_MODIFICADOCUANDO            "ModificadoCuando"


#define	TXT_CAMPO                           "Campo"
#define	TXT_CAMPO_VARIABLE                  "Variable"
#define	TXT_NOMBRE_CAMPO                    "Nombre"
#define	TXT_TIPO_CAMPO                      "Tipo"
#define	TXT_ORIGEN_VALOR_CAMPO              "OrigenValor"
#define	TXT_VALOR_CAMPO                     "Valor"
#define	TXT_FUENTE_CAMPO                    "Fuente"
#define	TXT_ESTADO_CAMPO                    "Estado"
#define	TXT_COORDENADA_X_CAMPO              "CoordenadaX"
#define	TXT_COORDENADA_Y_CAMPO              "CoordenadaY"
#define	TXT_TAMANO_CAMPO                    "TamanoFuente"
#define	TXT_NUM_CARACTERES_CAMPO            "NumCaracteres"
#define	TXT_ORIENTACION_CAMPO               "OrientacionFuente"
#define	TXT_ESTILO_CAMPO                    "Estilo"
#define	TXT_FILAS_MATRIZ_CAMPO              "Filas"
#define	TXT_COLUMNAS_MATRIZ_CAMPO           "Columnas"
#define	TXT_FREQ_FILAS_MATRIZ_CAMPO         "Frecuencia_filas"
#define	TXT_FREQ_COLUMNAS_MATRIZ_CAMPO      "Frecuencia_columnas"
#define	TXT_INC_COLUMNAS_MATRIZ_CAMPO      "inc_columnas"
#define	TXT_INC_FILAS_MATRIZ_CAMPO      "inc_filas"
#define	TXT_EXCEPCIONES_MATRIZ_CAMPO        "Excepciones"
#define	TXT_COLOR_CAMPO                     "Color"

#define TXT_PLANTENABLED    "PlantEnabled"
#define TXT_PLANTWIDTH  "PlantWidth"
#define TXT_PLANTHEIGHT "PlantHeight"
#define TXT_PLANTOFFSETX    "PlantOffsetX"
#define TXT_PLANTOFFSETY    "PlantOffsetY"
#define TXT_PLANTDISTCOLS   "PlantDistCols"
#define TXT_PLANTCOLS   "PlantCols"
#define TXT_PLANTFILE   "PlantFile"


#define	TXT_ORIGEN                  "Origen"
#define	TXT_COORDENADA_X_ORIGEN		"CoordenadaX"
#define	TXT_COORDENADA_Y_ORIGEN		"CoordenadaY"

#define	TXT_COMPOSICION							"Composicion"
#define	TXT_WEBWIDTH_COMPOSICION				"WebWidth"
#define	TXT_WEBHEIGHT_COMPOSICION				"WebHeight"
#define TXT_COMENTARIO_COMPOSICION              "Comentario"
#define	TXT_FILAS_COMPOSICION					"Filas"
#define	TXT_COLUMNAS_COMPOSICION				"Columnas"
#define	TXT_SEPARACION_FILAS_COMPOSICION		"SeparacionFilas"
#define	TXT_SEPARACION_COLUMNAS_COMPOSICION		"SeparacionColumnas"
#define	TXT_ANGULO_COMPOSICION                  "Angulo"

#define TXT_MACULA_IZQDA                "Macula_izqda"
#define TXT_MACULA_DCHA                 "Macula_dcha"
#define	TXT_ESTADO_MACULA               "Estado"
#define	TXT_COORDENADA_X_MACULA         "CoordenadaX"
#define	TXT_COORDENADA_Y_MACULA         "CoordenadaY"
#define	TXT_ANCHO_MACULA                "Ancho"
#define	TXT_ALTO_MACULA                 "Alto"
#define	TXT_REPETICIONES_MACULA         "Repeticiones"
#define	TXT_FRECUENCIA_MACULA			"Frecuencia"
#define	TXT_COLOR_MACULA                "Color"

#define	TXT_COLOR                   "Color"
#define	TXT_TIPO_COLOR              "Tipo"
#define	TXT_VALOR_COLOR             "Valor"
#define TXT_ALG_SELECCION_COLOR     "AlgSeleccionColor"
#define TXT_GRAY_THRESHOLD          "GrayThreshold"
#define	SC_PLENOS                   "Plenos"
#define	SC_CUATRICOMIA              "Cuatricomia"
#define TXT_MASCARA_COLOR           "Mascara_Color"

#define	TXT_COORDENADA_X_COLOR		"CoordenadaX"
#define	TXT_COORDENADA_Y_COLOR		"CoordenadaY"

#define	TXT_DB                      "DatosVariables_db"
#define	TXT_DB_NOMBRE               "Nombre"
#define	TXT_DB_RUTA                 "Ruta"
#define	TXT_DB_TABLA_DATOS          "TablaDatos"

#define TXT_HASH_KEY                "HASH_KEY"
#define TXT_HASH_VALUE              "HashValue"

#define TXT_WIDTH_BARCODE           "Ancho"
#define TXT_HEIGHT_BARCODE          "Alto"
#define TXT_CODIFICACION_BARCODE    "Codificacion"
#define TXT_TEXT_ALIGNMENT          "PosTexto"
#define TXT_OPTION2_BARCODE         "Subtipo"
#define TXT_FGCOLOR_BARCODE         "FgColor"
#define TXT_BGCOLOR_BARCODE         "BgColor"
#define TXT_INPUTMODE_BARCODE       "InputMode"
#define TXT_BORDERTYPE_BARCODE      "BorderType"
#define TXT_BORDERWIDTH_BARCODE     "BorderWidth"
#define TXT_WHITESPACE_BARCODE      "Margen"
#define TXT_SCALE_BARCODE           "Escala"
#define TXT_FONTTYPE_BARCODE        "Fuente"
#define TXT_FONTSMALL_BARCODE       "FuenteSmall"
#define TXT_FONTLARGE_BARCODE       "FuenteLarge"
#define TXT_ASPECTRATIO_BARCODE     "AspectRatio"
#define TXT_ESCONDERTEXTO_BARCODE   "EsconderTexto"
#define TXT_SEGURIDAD_BARCODE        "ECCLevel"


#define	R_TEXTEDIT_ON		253
#define	G_TEXTEDIT_ON		252
#define	B_TEXTEDIT_ON		219
#define	A_TEXTEDIT_ON		255

#define	R_TEXTEDIT_OFF		204
#define	G_TEXTEDIT_OFF		204
#define	B_TEXTEDIT_OFF		204
#define	A_TEXTEDIT_OFF		255

#define	    LED_XCL_MODE_Stopped		1
#define	    LED_XCL_MODE_Filling		2
#define     LED_XCL_MODE_Starting_MP	3
#define	    LED_XCL_MODE_Starting_DP	4
#define	    LED_XCL_MODE_Starting_T		5
#define     LED_XCL_MODE_Started		6
#define	    LED_XCL_MODE_Idling			7
#define	    LED_XCL_MODE_Stopping_T		8
#define	    LED_XCL_MODE_Stopping_DP	9
#define	    LED_XCL_MODE_Stopping_MP	10
#define     LED_XCL_MODE_Venting		11

#define     SENSOR_SupplyTemp		1
#define     SENSOR_LevelRes         2
#define     SENSOR_SupplyPr         3
#define     SENSOR_ReturnPr         4
#define     SENSOR_MeniscusPr		5
#define     SENSOR_DifferentialPr	6
#define     SENSOR_ReturnTemp		7
#define     SENSOR_ResTemp          8
#define     LEDs                    9
#define     SWITCHES                10

#define     IMAGEN_SINESCALAR  0
#define     IMAGEN_S2W         1
#define     IMAGEN_S2H         2

//Mapa de memoria unico de la aplicacion
//extern MemoryMap *memoryMap;
extern QWidget *mainClass;

//extern QString RootUsername;	//Nombre de usuario de Superusuario
//extern QString RootPassword;	//Contraseña de Superusuario
//extern QString RootUsernameEncrypted;	//Nombre de usuario encriptado de Superusuario
//extern QString RootPasswordEncrypted;	//Contraseña encriptada de Superusuario


//Tipos de usuarios
/*#define     USER_STRANGE    0
#define     USER_PRINTER    1
#define     USER_ROOT       2
#define     USER_GOD        3*/

//Eventos a registrar
#define     INICIO_SESION                   0
#define     FIN_SESION                      1
#define     CREA_COMPOSICION                2
#define     ERROR_MAQUINA                   3
#define     ERROR_HYDRA                     4
#define     MENSAJE_DEBUG                   5
#define     ERROR_LAMPARA                   6
#define     ERROR_GUI                       7
#define     ERROR_MANTENIMIENTO             8
#define     PARO_MAQUINA                    9
#define     ENVIA_COMPOSICION               10
#define     BORRAR_REGISTRO_MANTENIMIENTO   11
#define     BORRAR_REGISTROS_MANTENIMIENTO  12
#define     LIMPIEZA_AUTOMATICA_CABEZALES   13
#define     INICIO_IMPRESION                14
#define     FIN_IMPRESION                   15
#define     FALLO_LOG                       16

//Teclas del XUSB
/*#define TECLA_P     0
#define TECLA_R     1
#define TECLA_T     2

//Mensajes para/desde el microcontrolador
#define STOP_IMPRESION      0xFE
#define STOP_IMPRESION_0    0xF0
#define STOP_IMPRESION_1    0xF1
#define STOP_IMPRESION_2    0xF2
#define STOP_IMPRESION_3    0xF3
#define STOP_IMPRESION_4    0xF4
#define STOP_IMPRESION_5    0xF5
#define STOP_IMPRESION_6    0xF6
#define STOP_IMPRESION_7    0xF7

#define STOP_IMPRESION_ACK  0x0E
#define PAUSE_IMPRESION_ACK 0x0E

#define STOP_MAQUINA        0xA0
#define STOP_MAQUINA_ACK    0xA1

#define PAUSE_MAQUINA       0x90
#define PAUSE_MAQUINA_ACK   0x91

#define START_MAQUINA       0xE0
#define START_MAQUINA_ACK   0xE1

#define	LED_OFF             0xD0
#define	LED_OFF_ACK         0xD1

#define	LED_HOT             0xD2
#define	LED_HOT_ACK         0xD3

#define	LED_OK              0xD4
#define	LED_OK_ACK          0xD5*/





//Valores para el tipo de Campo Fijo/Variable
#define ID_CAMPO_TIPO_TEXTO            0
#define ID_CAMPO_TIPO_DATAMATRIX       1
#define ID_CAMPO_TIPO_IDEOGRAMA        2
#define ID_CAMPO_TIPO_FECHA            3
#define ID_CAMPO_TIPO_HORA             4

#define TXT_CAMPO_TIPO_TEXTO            "Texto"
#define TXT_CAMPO_TIPO_BARCODE          "Barcode"
#define TXT_CAMPO_TIPO_IDEOGRAMA        "Ideograma"

//Origen del valor para el tipo de Campo Variable
#define ID_ORIGEN_FIJO                 0
#define ID_ORIGEN_DB                   1
#define ID_ORIGEN_CONTADOR             2
#define ID_ORIGEN_FECHA                3
#define ID_ORIGEN_RANDOM               4

#define TXT_DILATE_ALG "DilateAlgorithm"
#define TXT_SABRE_ALG "SabreAlgorithm"
#define TXT_DOBLE_RES "DobleResolucion"
#define TXT_BITS_DITH "BitsDithering"
#define TXT_ORIGEN_FIJO          "Fijo"
#define TXT_ORIGEN_DB            "Db"
#define TXT_ORIGEN_CONTADOR      "Contador"
#define TXT_ORIGEN_FECHA         "Fecha/Hora"
#define TXT_ORIGEN_RANDOM        "Aleatorio"

#define TXT_CONTADOR_LIMITE_INFERIOR      "LimiteInferior"
#define TXT_CONTADOR_LIMITE_SUPERIOR      "LimiteSuperior"
#define TXT_CONTADOR_VALOR_INICIAL        "ValorInicial"
#define TXT_CONTADOR_INCREMENTO           "Incremento"
#define TXT_CONTADOR_CONFIGURACION        "Configuracion"
#define TXT_CONTADOR_OFFSET               "Offset"
#define TXT_CONTADOR_NUMERO_CIFRAS        "NumeroCifras"
#define TXT_CONTADOR_ZPADDING             "ZeroPadding"
#define TXT_CONTADOR_FORMATENABLED        "FormatEnabled"
#define TXT_CONTADOR_FORMATLANGUAJE        "FormatLanguaje"


#define TXT_CONTADOR_CONFIGURACION_FILAS        "PorFilas"
#define TXT_CONTADOR_CONFIGURACION_COLUMNAS     "PorColumnas"
#define ID_CONTADOR_CONFIGURACION_FILAS        0
#define ID_CONTADOR_CONFIGURACION_COLUMNAS     1

#define TXT_DATETIME_FORMATO             "Formato"

#define TXT_RANDOM_LIMITE_INFERIOR      "LimiteInferior"
#define TXT_RANDOM_LIMITE_SUPERIOR      "LimiteSuperior"
#define TXT_RANDOM_NUMERO_CIFRAS        "NumeroCifras"

#define HASH_ID "\nHASH:"




#define N_ITEMS_TABLA_INFORMER 32
#define INFORMER_DEBUG      0
#define INFORMER_WARNING    1
#define INFORMER_ERROR      2
#define INFORMER_WRITE      3

#define UA_NONE                     0
#define UA_CONFIGURACION_CARGADA    1
#define UA_IMAGEN_ENVIADA           2
#define UA_CONFIGURACION_GENERADA   3
#define UA_IMAGEN_SELECCIONADA      4
#define UA_TRABAJO_SELECCIONADO     5

#define HORAS_USO_LAMPARA_DEFAULT                  4000
#define DIAS_USO_FILTRO_DEFAULT                    180
#define TEMPERATURA_MAXIMA_LAMPARA_DEFAULT         40
#define DIAS_MAX_LIMPIEZA_LAMPARA_DEFAULT          8
#define DEFAULT_FRENO                              40
#define DEFAULT_TIEMPOPARADA                       10000
#define DEFAULT_ANCHO                               333
#define DEFAULT_DIAMETRO_SALIDA                     90
#define DEFAULT_DIAMETRO_ENTRADA                     300
//Ancho y largo de web por defecto
#define DEFAULT_WEB_WIDTH 300
#define DEFAULT_WEB_HEIGHT 150
#define DEFAULT_DISTANCIA_STOP 2000

#define TIMEOUT_REFRESH_OPC 1000
//#define TIMEOUT_TEST_SAI 10000
/*#ifdef HAPPA
    #define TIMEOUT_PRINT_COMPLETE 6000
    #define TIMEOUT_PRINT_COMPLETE_LONG 6000
#else
    #define TIMEOUT_PRINT_COMPLETE 14000
    #define TIMEOUT_PRINT_COMPLETE_LONG 25000
#endif*/

//Cabezal para preguntar
#define DEFAULT_CONTROL_HEAD 0
//Numero de errores consecutivos de medicion de diametro de entrada
#define CONTADOR_ERROR_DIAMETRO 3
//Diferencia de error de velocidad entre la medida y la referencia
#define DISCREPANCIA_VELOCIDAD 2
//A partir de este diametro en arranque, se limitra velocidad maxima
#define DIAMETRO_MINIMO_ARRANQUE 98
//Dimetro de arranque
#define DIAMETRO_ARRANQUE 250
//Velicdad maxima limitada por diametro de arranque
#define MAX_VELOCIDAD_DIAMETER_LIMITED 20
//Impresiones para empezar a frenar
#define IMPRESIONES_FRENADO 35
//Velocidad de frenado
#define VELOCIDAD_FRENADO 12

extern unsigned char flag_Hydra_ok;
extern unsigned char flag_Hydra_ok_ant;
extern unsigned char flag_Maquina_ok;
extern unsigned char flag_Maquina_ok_ant;

extern unsigned char flag_comunicacion_Hydra_ok;
extern unsigned char flag_temperatura_Hydra_ok;
extern unsigned char flag_nivel_Hydra_ok;
extern unsigned char flag_fill_Hydra_ok;

extern unsigned char flag_led_ok;

extern unsigned char flag_led_Maquina_rojo;
extern unsigned char flag_led_Lampara_rojo;

extern unsigned char codigo_error;

extern unsigned char tecla_XUSB;

extern unsigned char flag_start_Hydra_realizado;
extern unsigned char flag_shutdown_Hydra_realizado;
extern unsigned char flag_restart_Hydra_realizado;
extern unsigned char flag_descanso_Hydra;


extern int NumeroVentanas;
extern HWND HandleXUSB;
extern bool VentanaXUSBEncontrada;
//extern unsigned char flag_esperar_PrintComplete;

extern unsigned char flag_cambio_user_mode;

//extern short Buffer_Nivel_Hydra[5];
extern unsigned char index_nivel_Hydra;
//extern short Nivel_Hydra_filtrado;
//extern float Nivel_Hydra_filtrado_f;

extern unsigned char contador_comunicacion_Hydra_ko;

extern unsigned char escalado;
extern unsigned int cociente_w;
extern unsigned int cociente_h;
extern unsigned int factor_escala;


//extern QString AbsolutePathDbLog;
extern QString AbsolutePathDbColaImpresion;
//extern GestorDBODBC miDataBaseCampos;
//extern GestorDBSQLITE miDataBaseLog;
extern GestorDBSQLITE miDataBaseColaImpresion;

extern QStringList EstiloFuente;
extern QList<int> EstiloFuenteQt;

extern unsigned char NivelDebug;

extern unsigned char MantenimientoPendiente;
//extern unsigned char flag_repintar_principal;

extern unsigned char flag_error_iniciando_hydra;

extern unsigned char flag_referencia_velocidad_alcanzada;




#define MASCARA_FILTROS            0x01
#define MASCARA_HORAS_LAMPARA      0x02
#define MASCARA_DIA_LAMPARA        0x04
#define MASCARA_LIMPIEZA_LAMPARA   0x08

//extern unsigned char flag_entrando_en_modo_automatico;
//extern unsigned char flag_entrando_en_modo_manual;
//extern unsigned char flag_inicio_busqueda_macula;
//extern unsigned char flag_esperar_fin_busqueda_macula;
//extern unsigned char flag_mostrar_ventana_imagen_enviada;
//extern unsigned char flag_desplazar_film;
//extern unsigned char flag_fin_desplazamiento_film;

//Deberiamos hacer esto automatico, segun el tamaño de imagen que se genera
#define SIZE_BUFFER_IMAGES 32
#define VARIABLE_OFFSET_COLUMNA 2000//4000
#define VARIABLE_MAX_SWATHE_PRINT 334//667

//Valores para tipo_Movimiento para la función de ajuste de intensidad de lampara
#define MOVIMIENTO_AUTOMATICO    0
#define MOVIMIENTO_JOGMAS        1
#define MOVIMIENTO_JOGMENOS      2
#define MOVIMIENTO_MACULA        3

//Por defecto los pl a usar
#define DEFAULT_VOL 18
//Por defecto tamaño de gota
#define DEFAULT_DROP_VOL 6
//Temperaturas para estar en standby o working con los hydras
#define STANDBY_HYDRATEMPERATURE 25
#define WORKING_HYDRATEMPERATURE 45






void _Sleep(int iTimeout);  // Implementacion de sleep
void CleanQueueMsg();       // Limpia la cola de mensajes de Qt
void AbrirFicheroIni();
void GuardarFicheroIni();
void Inicializar_textos();
void GeneracionRutaPlantilla();
//void Convert();
//void ConvertC();
//void ConvertGS();
//void ConvertGS_Color();
//void ConvertGS_Color_Persistente();
void deleteTempGSFile();
void Inicializar_posicion_colores();
void LeerHydra();
//void Estado_Hydra();
void InicializarHydra();
void Inicializar_OPC();
void LeerXML();

void AbrirBaseDeDatosLog();
void CerrarBaseDeDatosLog();
void ComprobarIntegridadTablaUsuarios();
void HacerCopiaDeSeguridadLog();
void AplicarLimiteTamanoDbLog();
QStringList GetNombreColumnasLog();

void PruebaTonta();

//unsigned char Consultar_derechos_usuario();
//unsigned char  Consultar_usuario_registrado();
bool TestSuperUser();
//void Insertar_Evento(unsigned char tipo,QString valor);

unsigned char EscaladoDeImagenEnQLabel(int lw, int lh, int iw, int ih);

//void AbrirFicheroIniImpresora();
void GuardarFicheroIniImpresora();
//void AbrirFicheroParametrosImpresora();
void GuardarFicheroParametrosImpresora();

void CallBackFromDLL(DWORD Card, DWORD SentID);

QByteArray CalculateHash(QByteArray DatosEncriptar);
QByteArray GenerateHashFromFile(const QString &strFile);
bool TestHash(QByteArray DatosEncriptar,QByteArray ClaveEncriptada);
bool ComprobarIntegridadDb(QString AbsolutePathDb,QString NombreTabla);
void RecuperarBasedeDatos(QString AbsolutePathDb);
void BackupBasedeDatos(QString AbsolutePathDb);

void DiaLimpezaLampara(void);
void ComprobarOperacionesMantenimiento(void);

void Leer_Tabla_RefVelocidad_Intensidad();
void Leer_Tabla_RefVelocidad_Intensidad_2C();

//Establece cual es la pantalla principal
void GlobalSetMainWindow(QWidget *pMainWindow);
//Muestra la pantalla principal
void GlobalShowMainWindow();
//Oculta la pantalla principal
void GlobalHideMainWindow();
//Devuelve un puntero a la aplicacion principal
QWidget* GlobalGetMainWindow();

QString DevuelveNombre(QString NombreRuta);
QString DevuelveRuta(QString NombreRuta);
QString DevuelveNombreRutaIni(QString NombreRutaBmp);
void GlobalSetScreenSize(QWidget *p);
QString DevuelveExtension(QString NombreRuta);

int Relacion_VelocidadIntensidadLampara(int Velocidad);
int Relacion_VelocidadRampaAceleracion(int Velocidad);
int Relacion_VelocidadRampaDeceleracion(int Velocidad);
int Relacion_VelocidadTiempoBusqueda(int Velocidad);


void PauseHydra(int i);
void CloseHydra(int i);
void ActivateHydra(int i);
QLabel *ShowMensaje(QString texto);
void RegistraFuncion(const QString & qstr);
void ChangeStyle(int theme);
QPalette GetPaletaTextEditEnabled();
QPalette GetPaletaTextEditDisabled();
void SetApplicationDirPath(const QString & appPath);
QString GetApplicationDirPath();
QString _CopiarBBDD();
QString _GetBBDD();
QString _GenerateInternalBBDD();
void EjecutaPostProcesado();

void GenerateImpresionFileLck(const QString &filename);
QString ReturnImpresionFileLck(const QString &filename);
void SetHashCompoFile();
QString CalculateHashCompoFile();
QString GetHashCompoFile();
int generarNumeroAleatorio(int limiteInferior,int limiteSuperior);
void cambiarSemillaNumeroAleatorio(void);
void PruebaContador(int index);
int getTiempoParada();
QPixmap getPlantImage();
#endif //GLOBALS_H
