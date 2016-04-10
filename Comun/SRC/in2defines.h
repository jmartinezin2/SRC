#ifndef IN2DEFINES_H
#define IN2DEFINES_H

#include <QString>
#include <QStringList>
#include <QImage>
//#include "ScorpionDLL-ext.h"
#include <windows.h>

//Para los carteles auxiliares
#define AUX_INFO_WIDTH 200
#define AUX_INFO_HEIGHT 75

#define AUX_INFO_X  (1024-AUX_INFO_WIDTH)/2
#define AUX_INFO_Y  (768-AUX_INFO_HEIGHT)/2


#define  ROOTUSERNAMEENCRYPTED "ed0225b8b69a7dd03470f0f5f66bfe1ef0e06908"	//Nombre de usuario encriptado de Superusuario
//Contraseña encriptada de SuperUser por defecto
#define  ROOTPASSWORDENCRYPTED  "61768db8d1a38f1c16d3e6eea812ef423c739068"	//Contraseña encriptada de Superusuario->ABC123
//Para Esteve->In2Esteve
#define  ROOTPASSWORDENCRYPTED_ESTEVE    "a9e430319a6f2d02b78771eb6c844f419d9815a2"	//Contraseña encriptada de Superusuario->In2Esteve
//Contraseña solo para IN2
#define  ROOTPASSWORDIN2    "In2_1305"

#define CLIENT_IN2 "IN2"
#define CLIENT_ESTEVE "ESTEVE"
#define CLIENT_CINFAPILS "CINFAPILLS"
#define CLIENT_GRAMAR   "GRAMAR"

//Sensores de diametro
#define IN2MANAGER_DIN 0
#define IN2MANAGER_DOUT 1


//Distancia de macula para tener en cuenta
#define MIN_MACULA 2000000000
#define MAX_MACULA 0XFFFFFFFF

#define MACULA_MAXIMA 0
#define MACULA_MINIMA 1
#define ERROR_MACULA_MAX 0.72
#define ERROR_MACULA_MIN 0.29

//Algoritmos de dithering
#define FLOYD_STEINBERG 0xFF

//Timeout para aceptar fallos
#define START_TIMED 15000

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
#define TIME_HEAD_TEST 5000

//Encoder de limpieza
#define DEFAULT_CLEAN_ENCODER 20000

//Tamaños de letras
#define QFONT6	6
#define QFONT7	7
#define QFONT8	8
#define QFONT9	9
#define QFONT10	10
#define QFONT11	11
#define QFONT12	12



/*#define BUTTONW	110*FACTOR
#define BUTTONH  90*FACTOR_Y*/

//Los diferentes temas que podemos dibujar
enum TEMAS{
    THEME_NORMAL=0,
    THEME_FLUID_OK,
    THEME_FLUID_WARNING,
    THEME_EMERGENCY,
    THEME_PRINTING,
    THEME_CLEANING
};

//Modos de fin
enum FINIMPRESION{
    FIN_EMERGENCIA=0,
    FIN_LAMPARA,
    FIN_SAI,
    FIN_PULSADOR,
    FIN_METROS,
    FIN_DIAMETRO,
    FIN_ELECTRONICA,
    FIN_CABEZALES,
    FIN_NO_CONDITIONS,
    FIN_VISION,
    FIN_MACULA,
    FIN_LAMINA_ROTA,
    FIN_OPC_ALARM,
    FIN_NUMIMPRESIONES,
    FIN_FORCE_REPORT

};

//Niveles en el xaar
const int XAAR_8LEVELS[9] = {0,8,54,90,126,162,198,234,256};
const int XAAR_8VALUES[8] = {255,205,175,135,100,75,30,0};


//IDs de emergencias

#define EMERGENCIA_SETA                 0x01
#define EMERGENCIA_VARSALIDA            0x02
#define EMERGENCIA_VARENTRADA           0x04
#define EMERGENCIA_ARRASTRE             0x08
#define EMERGENCIA_BANDEJA              0x10
#define EMERGENCIA_MESA                 0x20
#define EMERGENCIA_CAPOTA               0x40
#define EMERGENCIA_PRESION              0x80
#define EMERGENCIA_LAMPARA_ELECTRICO    0x100
#define EMERGENCIA_LAMPARA_APAGADA     0x200
#define EMERGENCIA_DESPLAZADOR_LAMPARA     0x400


//Se definen leds aqui
#define LED_ROJO ":res/images/Ledrojo.png"
#define LED_VERDE ":res/images/Ledverde.png"


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
#define XUSBCLEANXML "c:\\xaar\\XUSBClean.xml"
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
#define TXT_LETTER_SPACE                    "LetterSpace"

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

#define TXT_MARCASUP "MARCASUP"
#define TXT_MARCAINF "MARCAINF"



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
#define TXT_ALG_DITH                "AlgDith"
#define TXT_ALG_WHITE               "AlgWhite"
#define TXT_ALG_BLACK               "AlgBlack"
#define TXT_ALG_MONO                "AlgMono"
#define TXT_ALG_CORRDIN             "AlgCorrDinamica"
#define TXT_ALG_CORRMONO            "AlgCorrMono"
#define TXT_COLOR_INDEX             "ColorIndex"
#define TXT_COLOR_DITHERCUT         "DitherCut"
#define TXT_COLOR_DITHERPAL         "DitherPal"
#define TXT_COLOR_CUSTOMPAL         "CustomPal"

#define TXT_GRAY_THRESHOLD          "GrayThreshold"
#define	SC_PLENOS                   "Plenos"
#define	SC_CUATRICOMIA              "Cuatricomia"
#define TXT_MASCARA_COLOR           "Mascara_Color"

#define	TXT_COORDENADA_X_COLOR		"CoordenadaX"
#define	TXT_COORDENADA_Y_COLOR		"CoordenadaY"
#define TXT_CADENA_COLOR            "CadenaColor"
#define TXT_ENABLED                 "enabled"

#define	TXT_DB                      "DatosVariables_db"
#define	TXT_DB_NOMBRE               "Nombre"
#define	TXT_DB_RUTA                 "Ruta"
#define	TXT_DB_TABLA_DATOS          "TablaDatos"
#define TXT_MAQUINA                 "Maquina"
#define TXT_VELOCIDAD               "Velocidad"
#define TXT_PAL1                    "Palette1"
#define TXT_PAL2                    "Palette2"
#define TXT_PAL3                    "Palette3"
#define TXT_PAL4                    "Palette4"
#define TXT_HASH_KEY                "HASH_KEY"
#define TXT_HASH_VALUE              "HashValue"

#define TXT_WIDTH_BARCODE           "Ancho"
#define TXT_HEIGHT_BARCODE          "Alto"
#define TXT_WIDTH_BARCODE_MM           "Ancho_mm"
#define TXT_HEIGHT_BARCODE_MM          "Alto_mm"
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


#define DEFAULT_SPEED 45
#define DEFAULT_XPALETTE "18@18@18@18@18@18@18@0"

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
#define TECLA_P     0
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
#define	LED_OK_ACK          0xD5

//Offset del filtro de macula
#define MACULA_OFFSET 10000
#define MACULA_FILTER 5

#define	REF_VELOCIDAD_MIN               5
#define	REF_VELOCIDAD_MAX               50
#define	REF_VELOCIDAD_MIN_2C            5
#define	REF_VELOCIDAD_MAX_2C            50
#define	REF_VELOCIDAD_MIN_MM            5000
#define	REF_VELOCIDAD_MAX_MM            100000
#define	METROS_TRABAJO_MAX              5000
#define	METROS_TRABAJO_MIN              1
#define	DIAMETRO_BOBINA_ENTRADA_MIN     75
#define	DIAMETRO_BOBINA_ENTRADA_MAX     450
#define	DIAMETRO_BOBINA_SALIDA_MIN      75
#define	DIAMETRO_BOBINA_SALIDA_MAX      450
#define	DIAMETRO_PARO_BOBINA_MIN        82
#define	DIAMETRO_PARO_BOBINA_MAX        450
#define DIAMETRO_MINIMO_BULTO_ENTRADA   100
#define DIAMETRO_MINIMO_BULTO_SALIDA    100

#define DIAM_LIMIT_EMPALME 120
#define CORRECION_METROS 0.9

#define	REF_TIEMPO_BUSQUEDA_MACULA_MIN      1
#define	REF_TIEMPO_BUSQUEDA_MACULA_MAX      60
#define	REF_VELOCIDAD_BUSQUEDA_MACULA_MIN   500
#define	REF_VELOCIDAD_BUSQUEDA_MACULA_MAX   5000
#define	REF_VELOCIDAD_AJUSTE_MACULA_MIN     100
#define	REF_VELOCIDAD_AJUSTE_MACULA_MAX     2000
#define	REF_VELOCIDAD_JOG_MAS_MIN           500
#define	REF_VELOCIDAD_JOG_MAS_MAX           10000
#define	REF_VELOCIDAD_JOG_MENOS_MIN         500
#define	REF_VELOCIDAD_JOG_MENOS_MAX         10000
#define	REF_FUERZA_FRENO_MIN                0
#define	REF_FUERZA_FRENO_MAX                100
#define	REF_INTENSIDAD_LAMPARA_MIN          0
#define	REF_INTENSIDAD_LAMPARA_MAX          100
#define	REF_TENSION_BOBINA_MIN              0
#define	REF_TENSION_BOBINA_MAX              100
#define	REF_RAMPA_ACELERACION_MIN           100
#define	REF_RAMPA_ACELERACION_MAX           2000
#define	REF_RAMPA_DECELERACION_MIN          100
#define	REF_RAMPA_DECELERACION_MAX          2000
#define	REF_DISTANCIA_EMPALME_MIN           0
#define	REF_DISTANCIA_EMPALME_MAX           500



/*
#define	ITEM_VELOCIDAD_ACT                0
#define	ITEM_VELOCIDAD_REF                1
#define	ITEM_METROS_ACT                   2
#define	ITEM_METROS_REF                   3
#define	ITEM_CONDICIONES_IMPRESION        4
#define	ITEM_TRABAJO_NUEVO                5
#define	ITEM_CONDICIONES_MOVIMIENTO       6
#define	ITEM_CODIGO_PARADA                7
#define	ITEM_PERMISO_PARAR                8
#define	ITEM_DIAMETRO_PARAR               9
#define	ITEM_PARO_MAQUINA                 10
#define	ITEM_MARCHA_MOVIMIENTO            11
#define	ITEM_PARO_MOVIMIENTO              12
#define	ITEM_SECUENCIA_ON                 13
#define	ITEM_DETECTOR_MACULA              14
#define	ITEM_JOG_MAS                      15
#define	ITEM_JOG_MENOS                    16
#define	ITEM_BUSCAR_MACULA                17
#define	ITEM_DESHABILITAR_SERVO_ENTRADA   18
#define	ITEM_DESHABILITAR_SERVO_SALIDA    19
#define	ITEM_DESPLAZAR_FILM               20
#define	ITEM_DESPLAZANDO_FILM             21
#define	ITEM_VELOCIDAD_AJUSTE_MACULA      22
#define	ITEM_VELOCIDAD_BUSQUEDA_MACULA    23
#define	ITEM_TIEMPO_BUSQUEDA_MACULA       24
#define	ITEM_VELOCIDAD_JOG_MAS            25
#define	ITEM_VELOCIDAD_JOG_MENOS          26
#define	ITEM_FRENO_ONOFF                  27
#define	ITEM_FUERZA_FRENO                 28
#define	ITEM_INTENSIDAD_LAMPARA           29
#define	ITEM_TEMPERATURA_LAMPARA          30
#define	ITEM_FIJAR_DIAMETRO_ENTRADA       31
#define	ITEM_FIJAR_DIAMETRO_SALIDA        32
#define	ITEM_DIAMETRO_ENTRADA_ACT         33
#define	ITEM_DIAMETRO_ENTRADA_REF         34
#define	ITEM_DIAMETRO_SALIDA_ACT          35
#define	ITEM_DIAMETRO_SALIDA_REF          36
#define	ITEM_TENSION_BOBINA               37
#define	ITEM_RAMPA_ACELERACION            38
#define	ITEM_RAMPA_DECELERACION           39
#define	ITEM_LAMPARA_ON                   40
#define	ITEM_DISTANCIA_EMPALME            41
#define	ITEM_BUSCANDO_MACULA              42
#define	ITEM_IMPRESION                    43
#define	ITEM_APAGAR_XUSB1                 44
#define	ITEM_APAGAR_XUSB2                 45
#define ITEM_ENABLE_CALCULO_DIAMETRO      46
#define ITEM_DIAMETRO_ENTRADA_FILTERED    47
#define ITEM_IMPRESION_REVERSE            48
#define ITEM_TENSAR_LAMINA                49
#define ITEM_DESTENSAR_LAMINA             50
#define ITEM_BALLUF_CENTRADO              51
#define ITEM_LAMINA_SIN_TENSION           52




#define	STRING_ITEM_VELOCIDAD_ACT          "ITEM_VELOCIDAD_ACT"
#define	STRING_ITEM_VELOCIDAD_REF          "ITEM_VELOCIDAD_REF"
#define	STRING_ITEM_METROS_ACT             "ITEM_METROS_ACT"
#define	STRING_ITEM_METROS_REF             "ITEM_METROS_REF"
#define	STRING_ITEM_CONDICIONES_IMPRESION  "ITEM_CONDICIONES_IMPRESION"
#define	STRING_ITEM_TRABAJO_NUEVO          "ITEM_TRABAJO_NUEVO"
#define	STRING_ITEM_CONDICIONES_MOVIMIENTO "ITEM_CONDICIONES_MOVIMIENTO"
#define	STRING_ITEM_CODIGO_PARADA          "ITEM_CODIGO_PARADA"
#define	STRING_ITEM_PERMISO_PARAR          "ITEM_PERMISO_PARAR"
#define	STRING_ITEM_DIAMETRO_PARAR         "ITEM_DIAMETRO_PARAR"
#define	STRING_ITEM_PARO_MAQUINA           "ITEM_PARO_MAQUINA"
#define	STRING_ITEM_MARCHA_MOVIMIENTO      "ITEM_MARCHA_MOVIMIENTO"
#define	STRING_ITEM_PARO_MOVIMIENTO        "ITEM_PARO_MOVIMIENTO"
#define	STRING_ITEM_SECUENCIA_ON           "ITEM_SECUENCIA_ON"
#define	STRING_ITEM_DETECTOR_MACULA        "ITEM_DETECTOR_MACULA"
#define	STRING_ITEM_JOG_MAS                "ITEM_JOG_MAS"
#define	STRING_ITEM_JOG_MENOS              "ITEM_JOG_MENOS"
#define	STRING_ITEM_BUSCAR_MACULA          "ITEM_BUSCAR_MACULA"
#define	STRING_ITEM_DESHABILITAR_SERVO_ENTRADA     "ITEM_DESHABILITAR_SERVO_ENTRADA"
#define	STRING_ITEM_DESHABILITAR_SERVO_SALIDA      "ITEM_DESHABILITAR_SERVO_SALIDA"
#define	STRING_ITEM_DESPLAZAR_FILM                 "ITEM_DESPLAZAR_FILM"
#define	STRING_ITEM_DESPLAZANDO_FILM               "ITEM_DESPLAZANDO_FILM"
#define	STRING_ITEM_VELOCIDAD_AJUSTE_MACULA        "ITEM_VELOCIDAD_AJUSTE_MACULA"
#define	STRING_ITEM_VELOCIDAD_BUSQUEDA_MACULA      "ITEM_VELOCIDAD_BUSQUEDA_MACULA"
#define	STRING_ITEM_TIEMPO_BUSQUEDA_MACULA         "ITEM_TIEMPO_BUSQUEDA_MACULA"
#define	STRING_ITEM_VELOCIDAD_JOG_MAS              "ITEM_VELOCIDAD_JOG_MAS"
#define	STRING_ITEM_VELOCIDAD_JOG_MENOS            "ITEM_VELOCIDAD_JOG_MENOS"
#define	STRING_ITEM_FRENO_ONOFF                    "ITEM_FRENO_ONOFF"
#define	STRING_ITEM_FUERZA_FRENO                   "ITEM_FUERZA_FRENO"
#define	STRING_ITEM_INTENSIDAD_LAMPARA             "ITEM_INTENSIDAD_LAMPARA"
#define	STRING_ITEM_TEMPERATURA_LAMPARA            "ITEM_TEMPERATURA_LAMPARA"
#define	STRING_ITEM_FIJAR_DIAMETRO_ENTRADA         "ITEM_FIJAR_DIAMETRO_ENTRADA"
#define	STRING_ITEM_FIJAR_DIAMETRO_SALIDA          "ITEM_FIJAR_DIAMETRO_SALIDA"
#define	STRING_ITEM_DIAMETRO_ENTRADA_ACT           "ITEM_DIAMETRO_ENTRADA_ACT"
#define	STRING_ITEM_DIAMETRO_ENTRADA_REF           "ITEM_DIAMETRO_ENTRADA_REF"
#define	STRING_ITEM_DIAMETRO_SALIDA_ACT            "ITEM_DIAMETRO_SALIDA_ACT"
#define	STRING_ITEM_DIAMETRO_SALIDA_REF            "ITEM_DIAMETRO_SALIDA_REF"
#define	STRING_ITEM_TENSION_BOBINA                 "ITEM_TENSION_BOBINA"
#define	STRING_ITEM_RAMPA_ACELERACION              "ITEM_RAMPA_ACELERACION"
#define	STRING_ITEM_RAMPA_DECELERACION             "ITEM_RAMPA_DECELERACION"
#define	STRING_ITEM_LAMPARA_ON                     "ITEM_LAMPARA_ON"
#define	STRING_ITEM_DISTANCIA_EMPALME              "ITEM_DISTANCIA_EMPALME"
#define	STRING_ITEM_BUSCANDO_MACULA                "ITEM_BUSCANDO_MACULA"
#define	STRING_ITEM_IMPRESION                      "ITEM_IMPRESION"
#define	STRING_ITEM_APAGAR_XUSB1                   "ITEM_APAGAR_XUSB1"
#define	STRING_ITEM_APAGAR_XUSB2                   "ITEM_APAGAR_XUSB2"
#define STRING_ITEM_ENABLE_CALCULO_DIAMETRO        "ITEM_CALCULO_DIAMETRO"
#define STRING_ITEM_DIAMETRO_ENTRADA_FILTERED      "ITEM_DIAMETRO_ENTRADA_FILTERED"
#define STRING_ITEM_IMPRESION_REVERSE              "ITEM_IMPRESION_REVERSE"
#define STRING_ITEM_TENSAR_LAMINA                  "ITEM_TENSAR_LAMINA"
#define STRING_ITEM_DESTENSAR_LAMINA               "ITEM_DESTENSAR_LAMINA"
#define STRING_ITEM_BALLUF_CENTRADO                "ITEM_BALLUF_CENTRADO"
#define STRING_ITEM_LAMINA_SIN_TENSION             "ITEM_LAMINA_SIN_TENSION"


//ITEMS CINFA 1
#define	ID_VELOCIDAD_ACT            "n(172_16_170_6)_1.s1.m18.p02"
#define	ID_VELOCIDAD_REF            "n(172_16_170_6)_1.s1.m18.p15"
#define	ID_METROS_ACT               "n(172_16_170_6)_1.s1.m18.p21"
#define	ID_METROS_REF               "n(172_16_170_6)_1.s1.m18.p20"
#define	ID_CONDICIONES_IMPRESION	"n(172_16_170_6)_1.s1.m18.p34" //Se lee a 1 cuando podamos imprimir
#define	ID_TRABAJO_NUEVO            "n(172_16_170_6)_1.s1.m18.p35"
#define	ID_CONDICIONES_MOVIMIENTO	"n(172_16_170_6)_1.s1.m18.p36" //Lo pongo a 1 cuando s pueda mover
#define	ID_DIAMETRO_ENTRADA_ACT     "n(172_16_170_6)_1.s1.m18.p18"
#define	ID_DIAMETRO_ENTRADA_REF     "n(172_16_170_6)_1.s1.m18.p19"
#define	ID_CODIGO_PARADA            "n(172_16_170_6)_1.s1.m18.p23"
#define	ID_PERMISO_PARAR            "n(172_16_170_6)_1.s1.m18.p38" //A 1 cuando ya haya hecho el soft-stop
#define	ID_DIAMETRO_PARAR           "n(172_16_170_6)_1.s1.m18.p24"
#define	ID_FIJAR_DIAMETRO_ENTRADA   "n(172_16_170_6)_1.s1.m18.p32"


#ifdef DEPURACION_IN2
//ITEMS CINFA 2
#define	ID_CONDICIONES_MOVIMIENTO_EXT       "S7:[@LOCALSERVER]DB1X0.0" //Lo pongo a 1 cuando s pueda mover
#define	ID_PARO_MOVIMIENTO_EXT              "S7:[@LOCALSERVER]DB1X0.1"
#define	ID_MARCHA_MOVIMIENTO_EXT            "S7:[@LOCALSERVER]DB1X0.2"
#define	ID_PARO_MAQUINA_EXT                 "S7:[@LOCALSERVER]DB1X0.3"
#define	ID_JOG_MENOS_EXT                    "S7:[@LOCALSERVER]DB1X0.4"
#define	ID_JOG_MAS_EXT                      "S7:[@LOCALSERVER]DB1X0.5"
#define	ID_BUSCAR_MACULA_EXT                "S7:[@LOCALSERVER]DB1X0.6"
#define	ID_DESHABILITAR_SERVO_ENTRADA_EXT   "S7:[@LOCALSERVER]DB1X0.7"
#define	ID_DESHABILITAR_SERVO_SALIDA_EXT    "S7:[@LOCALSERVER]DB1X1.0"
#define	ID_DESPLAZAR_FILM_EXT               "S7:[@LOCALSERVER]DB1X1.1"
#define	ID_FRENO_ONOFF_EXT                  "S7:[@LOCALSERVER]DB1X1.2"
#define ID_FIJAR_DIAMETRO_ENTRADA_EXT       "S7:[@LOCALSERVER]DB1X1.3"
#define ID_FIJAR_DIAMETRO_SALIDA_EXT        "S7:[@LOCALSERVER]DB1X1.4"
#define	ID_TRABAJO_NUEVO_EXT                "S7:[@LOCALSERVER]DB1X1.5"
#define ID_RESET_MENSAJE_FALLO_EXT          "S7:[@LOCALSERVER]DB1X1.6"
#define ID_LAMPARA_ON_EXT                   "S7:[@LOCALSERVER]DB1X1.7"
#define ID_APAGAR_XUSB1_EXT                 "S7:[@LOCALSERVER]DB1X2.0"
#define ID_APAGAR_XUSB2_EXT                 "S7:[@LOCALSERVER]DB1X2.1"
#define ID_ENABLE_CALCULO_DIAMETRO          "S7:[@LOCALSERVER]DB100X2.7"
#define ID_IMPRESION_REVERSE                "S7:[@LOCALSERVER]DB100X3.0"
#define ID_TENSAR_LAMINA                    "S7:[@LOCALSERVER]DB100X3.1"
#define ID_DESTENSAR_LAMINA                 "S7:[@LOCALSERVER]DB100X3.2"
#define	ID_CONDICIONES_IMPRESION_EXT        "S7:[@LOCALSERVER]DB1X3.0" //Se lee a 1 cuando podamos imprimir
#define ID_BALLUF_CENTRADO                  "S7:[@LOCALSERVER]DB101X0.6"
#define ID_LAMINA_SIN_TENSION               "S7:[@LOCALSERVER]DB101X0.7"
#define	ID_SECUENCIA_ON_EXT                 "S7:[@LOCALSERVER]DB1X3.1"
#define	ID_DETECTOR_MACULA_EXT              "S7:[@LOCALSERVER]DB1X3.2"
#define ID_DESPLAZANDO_FILM_EXT             "S7:[@LOCALSERVER]DB1X3.3"
#define ID_BUSCANDO_MACULA_EXT              "S7:[@LOCALSERVER]DB1X3.4"
#define ID_IMPRESION_EXT                    "S7:[@LOCALSERVER]DB1X3.5"

#define	ID_VELOCIDAD_REF_EXT                "S7:[@LOCALSERVER]DB1W4"
#define	ID_INTENSIDAD_LAMPARA_EXT           "S7:[@LOCALSERVER]DB1W6"
#define	ID_FUERZA_FRENO_EXT                 "S7:[@LOCALSERVER]DB1W8"
#define	ID_METROS_REF_EXT                   "S7:[@LOCALSERVER]DB1W10"
#define	ID_DIAMETRO_ENTRADA_REF_EXT         "S7:[@LOCALSERVER]DB1W12"
#define ID_DIAMETRO_SALIDA_REF_EXT          "S7:[@LOCALSERVER]DB1W14"
#define	ID_TIEMPO_BUSQUEDA_MACULA_EXT       "S7:[@LOCALSERVER]DB1W16"
#define	ID_VELOCIDAD_BUSQUEDA_MACULA_EXT    "S7:[@LOCALSERVER]DB1W18"
#define	ID_VELOCIDAD_AJUSTE_MACULA_EXT      "S7:[@LOCALSERVER]DB1W20"
#define	ID_VELOCIDAD_JOG_MENOS_EXT          "S7:[@LOCALSERVER]DB1W22"
#define	ID_VELOCIDAD_JOG_MAS_EXT            "S7:[@LOCALSERVER]DB1W24"
#define ID_DISTANCIA_EMPALME_EXT            "S7:[@LOCALSERVER]DB1W26"
#define	ID_DIAMETRO_PARAR_EXT               "S7:[@LOCALSERVER]DB1W28"
#define ID_TENSION_BOBINA_EXT               "S7:[@LOCALSERVER]DB1W30"
#define ID_RAMPA_ACELERACION_EXT            "S7:[@LOCALSERVER]DB1W32"
#define ID_RAMPA_DECELERACION_EXT           "S7:[@LOCALSERVER]DB1W34"
#define ID_DIAMETRO_ENTRADA_FILTERED        "S7:[@LOCALSERVER]DB100W40"
#define	ID_METROS_ACT_EXT                               "S7:[@LOCALSERVER]DB1W36"
#define	ID_DIAMETRO_ENTRADA_ACT_EXT                     "S7:[@LOCALSERVER]DB1W38"
#define ID_DIAMETRO_SALIDA_ACT_EXT                      "S7:[@LOCALSERVER]DB1W40"
#define	ID_TEMPERATURA_LAMPARA_EXT                      "S7:[@LOCALSERVER]DB1W42"
#define	ID_VELOCIDAD_ACT_EXT                            "S7:[@LOCALSERVER]DB1W44"
#define ID_ERROR_STATUS_PROFINET_EXT                    "S7:[@LOCALSERVER]DB1W46"     //Errores PLC
#define ID_ERROR_STATUS_ENCODER_EXT                     "S7:[@LOCALSERVER]DB1W48"
#define ID_ERROR_STATUS_CPU_DRIVER_SALIDA_EXT           "S7:[@LOCALSERVER]DB1W50"
#define ID_ERROR_STATUS_CPU_DRIVER_ENTRADA_EXT          "S7:[@LOCALSERVER]DB1W52"
#define ID_ERROR_STATUS_DRIVER_SALIDA_EXT               "S7:[@LOCALSERVER]DB1W54"
#define ID_ERROR_STATUS_DRIVER_ENTRADA_EXT              "S7:[@LOCALSERVER]DB1W56"
#define	ID_CODIGO_PARADA_EXT                            "S7:[@LOCALSERVER]DB1W58"

#else

//ITEMS CINFA 2
#define	ID_CONDICIONES_MOVIMIENTO_EXT       "S7:[S7 connection_1]DB100X0.0" //Lo pongo a 1 cuando s pueda mover
#define	ID_PARO_MOVIMIENTO_EXT              "S7:[S7 connection_1]DB100X0.1"
#define	ID_MARCHA_MOVIMIENTO_EXT            "S7:[S7 connection_1]DB100X0.2"
#define	ID_PARO_MAQUINA_EXT                 "S7:[S7 connection_1]DB100X0.3"
#define	ID_JOG_MENOS_EXT                    "S7:[S7 connection_1]DB100X0.4"
#define	ID_JOG_MAS_EXT                      "S7:[S7 connection_1]DB100X0.5"
#define	ID_BUSCAR_MACULA_EXT                "S7:[S7 connection_1]DB100X0.6"
#define	ID_DESHABILITAR_SERVO_ENTRADA_EXT   "S7:[S7 connection_1]DB100X0.7"
#define	ID_DESHABILITAR_SERVO_SALIDA_EXT    "S7:[S7 connection_1]DB100X1.0"
#define	ID_DESPLAZAR_FILM_EXT               "S7:[S7 connection_1]DB100X1.1"
#define	ID_FRENO_ONOFF_EXT                  "S7:[S7 connection_1]DB100X1.2"
#define ID_FIJAR_DIAMETRO_ENTRADA_EXT       "S7:[S7 connection_1]DB100X1.3"
#define ID_FIJAR_DIAMETRO_SALIDA_EXT        "S7:[S7 connection_1]DB100X1.4"
#define	ID_TRABAJO_NUEVO_EXT                "S7:[S7 connection_1]DB100X1.5"
#define ID_RESET_MENSAJE_FALLO_EXT          "S7:[S7 connection_1]DB100X1.6"
#define ID_LAMPARA_ON_EXT                   "S7:[S7 connection_1]DB100X1.7"
#define ID_APAGAR_XUSB1_EXT                 "S7:[S7 connection_1]DB100X2.0"
#define ID_APAGAR_XUSB2_EXT                 "S7:[S7 connection_1]DB100X2.1"
#define ID_ENABLE_CALCULO_DIAMETRO          "S7:[S7 connection_1]DB100X2.7"

#define ID_IMPRESION_REVERSE                "S7:[S7 connection_1]DB100X3.0"
#define ID_TENSAR_LAMINA                    "S7:[S7 connection_1]DB100X3.1"
#define ID_DESTENSAR_LAMINA                 "S7:[S7 connection_1]DB100X3.2"

#define	ID_CONDICIONES_IMPRESION_EXT        "S7:[S7 connection_1]DB101X0.0" //Se lee a 1 cuando podamos imprimir
#define	ID_SECUENCIA_ON_EXT                 "S7:[S7 connection_1]DB101X0.1"
#define	ID_DETECTOR_MACULA_EXT              "S7:[S7 connection_1]DB101X0.2"
#define ID_DESPLAZANDO_FILM_EXT             "S7:[S7 connection_1]DB101X0.3"
#define ID_BUSCANDO_MACULA_EXT              "S7:[S7 connection_1]DB101X0.4"
#define ID_IMPRESION_EXT                    "S7:[S7 connection_1]DB101X0.5"
#define ID_BALLUF_CENTRADO                  "S7:[S7 connection_1]DB101X0.6"
#define ID_LAMINA_SIN_TENSION                  "S7:[S7 connection_1]DB101X0.7"
#define	ID_VELOCIDAD_REF_EXT                "S7:[S7 connection_1]DB100W4"
#define	ID_INTENSIDAD_LAMPARA_EXT           "S7:[S7 connection_1]DB100W6"
#define	ID_FUERZA_FRENO_EXT                 "S7:[S7 connection_1]DB100W8"
#define	ID_METROS_REF_EXT                   "S7:[S7 connection_1]DB100W10"
#define	ID_DIAMETRO_ENTRADA_REF_EXT         "S7:[S7 connection_1]DB100W12"
#define ID_DIAMETRO_SALIDA_REF_EXT          "S7:[S7 connection_1]DB100W14"
#define	ID_TIEMPO_BUSQUEDA_MACULA_EXT       "S7:[S7 connection_1]DB100W16"
#define	ID_VELOCIDAD_BUSQUEDA_MACULA_EXT    "S7:[S7 connection_1]DB100W18"
#define	ID_VELOCIDAD_AJUSTE_MACULA_EXT      "S7:[S7 connection_1]DB100W20"
#define	ID_VELOCIDAD_JOG_MENOS_EXT          "S7:[S7 connection_1]DB100W22"
#define	ID_VELOCIDAD_JOG_MAS_EXT            "S7:[S7 connection_1]DB100W24"
#define ID_DISTANCIA_EMPALME_EXT            "S7:[S7 connection_1]DB100W26"
#define	ID_DIAMETRO_PARAR_EXT               "S7:[S7 connection_1]DB100W28"
#define ID_TENSION_BOBINA_EXT               "S7:[S7 connection_1]DB100W30"
#define ID_RAMPA_ACELERACION_EXT            "S7:[S7 connection_1]DB100W32"
#define ID_RAMPA_DECELERACION_EXT           "S7:[S7 connection_1]DB100W34"
#define ID_DIAMETRO_ENTRADA_FILTERED        "S7:[S7 connection_1]DB100W40"
#define	ID_METROS_ACT_EXT                               "S7:[S7 connection_1]DB101W4"
#define	ID_DIAMETRO_ENTRADA_ACT_EXT                     "S7:[S7 connection_1]DB101W6"
#define ID_DIAMETRO_SALIDA_ACT_EXT                      "S7:[S7 connection_1]DB101W8"
#define	ID_TEMPERATURA_LAMPARA_EXT                      "S7:[S7 connection_1]DB101W10"
#define	ID_VELOCIDAD_ACT_EXT                            "S7:[S7 connection_1]DB101W12"
#define ID_ERROR_STATUS_PROFINET_EXT                    "S7:[S7 connection_1]DB101W14"     //Errores PLC
#define ID_ERROR_STATUS_ENCODER_EXT                     "S7:[S7 connection_1]DB101W16"
#define ID_ERROR_STATUS_CPU_DRIVER_SALIDA_EXT           "S7:[S7 connection_1]DB101W18"
#define ID_ERROR_STATUS_CPU_DRIVER_ENTRADA_EXT          "S7:[S7 connection_1]DB101W20"
#define ID_ERROR_STATUS_DRIVER_SALIDA_EXT               "S7:[S7 connection_1]DB101W22"
#define ID_ERROR_STATUS_DRIVER_ENTRADA_EXT              "S7:[S7 connection_1]DB101W24"
#define	ID_CODIGO_PARADA_EXT                            "S7:[S7 connection_1]DB101W26"
 1-Emergencia
 2-Variador salida nok
 3-Variador entrada nok
 4-Fin trabajo
 5-Fin bobina entrada
 6-Fin bobina salida
 7-Fallo arrastre
 8-Bandeja nok
 9-Mesa de impresión nok

#endif*/

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

//ALGORITMOS DE DIFUSION
#define TXT_DIFFUSE     "Diffuse"
#define TXT_ORDERED     "Ordered"
#define TXT_THRESHOLD   "Threshold"
#define TXT_FLOYD       "Floyd"


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

#define N_ITEMS_TABLA_DEBUG 4096
#define LOGGER_WARNING    0
#define LOGGER_DEBUG      1
#define LOGGER_ERROR      2
#define LOGGER_STORE      3
#define LOGGER_PARADA      4

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
 #ifdef X1001
    #define DEFAULT_HEAD_WIDTH 1000
    #define	FACTOR_PIXELS_MM		(float) 14.1844 //(float) 14.1732=360dpi/25.4mm
    #define	FACTOR_PIXELS_PUNTO		(float) 5.0039
#else
    #define DEFAULT_HEAD_WIDTH 382
    #define	FACTOR_PIXELS_MM		(float) 7.0922/*7.0922*/ //(float) 14.1732=360dpi/25.4mm
    #define	FACTOR_PIXELS_PUNTO		(float) 5.0039
#endif


#define TIMEOUT_REFRESH_OPC 1000
#define TIMEOUT_TEST_SAI 10000
#ifdef HAPPA
    #define TIMEOUT_PRINT_COMPLETE 6000
    #define TIMEOUT_PRINT_COMPLETE_LONG 6000
#else
    #define TIMEOUT_PRINT_COMPLETE 20000
    #define TIMEOUT_PRINT_COMPLETE_LONG 30000
#endif

#ifdef HAPPA
    //Tiempo de arranque tras empalme
    #define TIEMPO_ARRANQUE_EMPALME 17000
//Velocidad minima de arranque
    #define VEL_MIN_ARRANQUE 7
    //Tiempo de arranque a velocidad minima en msg
    #define TIEMPO_ARRANQUE 15000
    //Repeticiones para aumentar velocidad
    #define COUNTER_VELOCIDAD 10
    //Velocidad minima para control de velocidad
    #define MIN_CONTROL_VELOCIDAD 25
#else
    //Tiempo de arranque tras empalme
    #define TIEMPO_ARRANQUE_EMPALME 32000
    //Repeticiones para aumentar velocidad
    #define COUNTER_VELOCIDAD 2
    //Velocidad minima para control de velocidad
    #define MIN_CONTROL_VELOCIDAD 85
    //Velocidad minima de arranque
    #define VEL_MIN_ARRANQUE 1
    //Tiempo de arranque a velocidad minima en msg
    #define TIEMPO_ARRANQUE 15000

#endif
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

#define MASCARA_FILTROS            0x01
#define MASCARA_HORAS_LAMPARA      0x02
#define MASCARA_DIA_LAMPARA        0x04
#define MASCARA_LIMPIEZA_LAMPARA   0x08

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

//Estructura de control de imagen
typedef struct In2VarImage{
    QImage m_Imagen;
    QString file;
    char m_state;
    DWORD m_copyCount;
    int cabezal;
}IN2VARIMAGE;

//Estructura de trabajo
struct In2Trabajo{
    QString strCompoFile;
    QStringList strlstFiles;
    QString m_OT;
    int index;
    bool bActive;
    int iTotalToPrint;
    int iPrinted;
    int iNumGaps;
    int Offset;
    int elapsedTime;
    //QPixmap image;
    In2VarImage ImageInfo;
};

//Estados para la estructura In2VarImage
enum EstadosImagenXAAR{
    XAAR_IMAGEN_VACIA=0,
    XAAR_IMAGEN_CREADA,
    XAAR_IMAGEN_ENVIADA,
    XAAR_IMAGEN_BLANCA
};

#endif // IN2DEFINES_H

