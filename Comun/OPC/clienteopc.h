#ifndef CLIENTEOPC_H
#define CLIENTEOPC_H

#include <QString>
#include <QObject>
#include <QList>
#include <QVariant>
#include <QVector>
#include "cemitecproxyopc.h"
//#include "globals.h"
#include "MemoryMap.h"


//En caso de Happa, es para dejarle mas tiempo de curado*/
#define TIMEOUT_PARADA_DIAMETRO 8000

#define TIMEOUT_BUSCAMACULA 1000
//Para los flancos
#define DELAY_OPC_SIGNAL 250


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
#define ITEM_LAMPARA_VISION               51
#define ITEM_BALLUF_CENTRADO              52
#define ITEM_LAMINA_SIN_TENSION           53
#define ITEM_ERROR_BOBINADOR              54
#define ITEM_ERROR_DESBOBINADOR           55
#define ITEM_NUMERO_MACULAS               56
#define ITEM_MAX_DISTANCIA_MACULA         57
#define ITEM_MIN_DISTANCIA_MACULA         58
#define ITEM_DIAMETRO_SALIDA_FILTERED     59

#define ITEM_LAMPARA_MOVER                60
#define ITEM_LAMPARA_SETUP                61
#define ITEM_LAMPARA_RESET                62
#define ITEM_LAMPARA_JOGF                 63
#define ITEM_LAMPARA_JOGR                 64
#define ITEM_LAMPARA_POS                  65
#define ITEM_DESPLAZ_SET                  66
#define ITEM_DESPLAZ_INP                  67
#define ITEM_DIAM_PLC_OUTPUT              68
#define ITEM_DIAM_PLC_INPUT               69
#define ITEM_BAJAR_CUCHILLAS              70
#define ITEM_FUERZA_CUCHILLAS             71




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
#define STRING_ITEM_LAMPARA_VISION                 "ITEM_LAMPARA_VISION"
#define STRING_ITEM_BALLUF_CENTRADO                "ITEM_BALLUF_CENTRADO"
#define STRING_ITEM_LAMINA_SIN_TENSION             "ITEM_LAMINA_SIN_TENSION"
#define STRING_ITEM_ERROR_BOBINADOR                "ITEM_ERROR_BOBINADOR"
#define STRING_ITEM_ERROR_DESBOBINADOR             "ITEM_ERROR_DESBOBINADOR"
#define STRING_NUMERO_MACULAS                      "ITEM_NUMERO_MACULAS"
#define STRING_MAX_DISTANCIA_MACULA                "ITEM_MAXDISTANCIA_MACULA"
#define STRING_MIN_DISTANCIA_MACULA                "ITEM_MINDISTANCIA_MACULA"
#define STRING_DIAMETRO_SALIDA_FILTERED            "ITEM_DIAMETRO_SALIDA_FILTERED"

#define STRING_ITEM_LAMPARA_MOVER                "ITEM_LAMPARA_MOVER"
#define STRING_ITEM_LAMPARA_SETUP                "ITEM_LAMPARA_SETUP"
#define STRING_ITEM_LAMPARA_RESET                "ITEM_LAMPARA_RESET"
#define STRING_ITEM_LAMPARA_JOGF                 "ITEM_LAMPARA_JOGF"
#define STRING_ITEM_LAMPARA_JOGR                 "ITEM_LAMPARA_JOGR"
#define STRING_ITEM_LAMPARA_POS                  "ITEM_LAMPARA_POS"
#define STRING_ITEM_DESPLAZ_SET                  "ITEM_DESPLAZ_SET"
#define STRING_ITEM_DESPLAZ_INP                  "ITEM_DESPLZ_INP"
#define STRING_ITEM_DIAM_PLC_OUTPUT              "ITEM_DIAM_PLC_OUTPUT"
#define STRING_ITEM_DIAM_PLC_INPUT               "ITEM_DIAM_PLC_INPUT"
#define STRING_ITEM_BAJAR_CUCHILLAS              "ITEM_BAJAR_CUCHILLAS"
#define STRING_ITEM_FUERZA_CUCHILLAS             "ITEM_FUERZA_CUCHILLAS"



/*#define	REF_VELOCIDAD_MIN               5
#define	REF_VELOCIDAD_MAX               100
#define	REF_VELOCIDAD_MIN_2C            5
#define	REF_VELOCIDAD_MAX_2C            100
#define	REF_VELOCIDAD_MIN_MM            5000
#define	REF_VELOCIDAD_MAX_MM            100000
#define	METROS_TRABAJO_MAX              5000
#define	METROS_TRABAJO_MIN              1
#define	DIAMETRO_BOBINA_ENTRADA_MIN     75
#define	DIAMETRO_BOBINA_ENTRADA_MAX     1000*/

/*#define	DIAMETRO_BOBINA_SALIDA_MIN      75
#define	DIAMETRO_BOBINA_SALIDA_MAX      440
#define	DIAMETRO_PARO_BOBINA_MIN        75
#define	DIAMETRO_PARO_BOBINA_MAX        1000*/

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
#define ID_APAGAR_XUSB1_EXT                 "S7:[@LOCALSERVER]DB1X2.1"
#define ID_APAGAR_XUSB2_EXT                 "S7:[@LOCALSERVER]DB1X2.2"
#define ID_ENABLE_CALCULO_DIAMETRO          "S7:[@LOCALSERVER]DB1X2.3"
#define ID_IMPRESION_REVERSE                "S7:[@LOCALSERVER]DB1X2.4"
#define ID_TENSAR_LAMINA                    "S7:[@LOCALSERVER]DB1X2.5"
#define ID_DESTENSAR_LAMINA                 "S7:[@LOCALSERVER]DB1X2.6"
#define	ID_CONDICIONES_IMPRESION_EXT        "S7:[@LOCALSERVER]DB1X2.7" //Se lee a 1 cuando podamos imprimir
#define ID_BALLUF_CENTRADO                  "S7:[@LOCALSERVER]DB1X3.0"
#define ID_LAMINA_SIN_TENSION               "S7:[@LOCALSERVER]DB1X4.1"
#define	ID_SECUENCIA_ON_EXT                 "S7:[@LOCALSERVER]DB1X3.2"
#define	ID_DETECTOR_MACULA_EXT              "S7:[@LOCALSERVER]DB1X3.3"
#define ID_DESPLAZANDO_FILM_EXT             "S7:[@LOCALSERVER]DB1X3.4"
#define ID_BUSCANDO_MACULA_EXT              "S7:[@LOCALSERVER]DB1X3.5"
#define ID_IMPRESION_EXT                    "S7:[@LOCALSERVER]DB1X3.6"
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
#define ID_DIAMETRO_ENTRADA_FILTERED        "S7:[@LOCALSERVER]DB1W36"
#define	ID_METROS_ACT_EXT                               "S7:[@LOCALSERVER]DB1W38"
#define	ID_DIAMETRO_ENTRADA_ACT_EXT                     "S7:[@LOCALSERVER]DB1W40"
#define ID_DIAMETRO_SALIDA_ACT_EXT                      "S7:[@LOCALSERVER]DB1W42"
#define	ID_TEMPERATURA_LAMPARA_EXT                      "S7:[@LOCALSERVER]DB1W44"
#define	ID_VELOCIDAD_ACT_EXT                            "S7:[@LOCALSERVER]DB1W46"
#define ID_ERROR_STATUS_PROFINET_EXT                    "S7:[@LOCALSERVER]DB1W48"     //Errores PLC
#define ID_ERROR_STATUS_ENCODER_EXT                     "S7:[@LOCALSERVER]DB1W50"
#define ID_ERROR_STATUS_CPU_DRIVER_SALIDA_EXT           "S7:[@LOCALSERVER]DB1W52"
#define ID_ERROR_STATUS_CPU_DRIVER_ENTRADA_EXT          "S7:[@LOCALSERVER]DB1W54"
#define ID_ERROR_STATUS_DRIVER_SALIDA_EXT               "S7:[@LOCALSERVER]DB1W56"
#define ID_ERROR_STATUS_DRIVER_ENTRADA_EXT              "S7:[@LOCALSERVER]DB1W58"
#define	ID_CODIGO_PARADA_EXT                            "S7:[@LOCALSERVER]DB1W60"
#define ID_ERROR_BOBINADOR                              "S7:[@LOCALSERVER]DB1W62"
#define ID_ERROR_DESBOBINADOR                           "S7:[@LOCALSERVER]DB1W64"
#define ID_DIAMETRO_SALIDA_FILTERED                     "S7:[@LOCALSERVER]DB1W66"
#define ID_NUMERO_MACULAS                               "S7:[@LOCALSERVER]DB1W68"
#define ID_MAX_DISTANCIA_MACULA                         "S7:[@LOCALSERVER]DB1W70"
#define ID_MIN_DISTANCIA_MACULA                         "S7:[@LOCALSERVER]DB1W72"
#define ID_LAMPARA_VISION                               "S7:[@LOCALSERVER]DB1W74"


#else

//DB100
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
#define ID_BAJAR_CUCHILLAS                  "S7:[S7 connection_1]DB100X2.5"
#define ID_LAMPARA_MOVER                    "S7:[S7 connection_1]DB100X2.6"
#define ID_ENABLE_CALCULO_DIAMETRO          "S7:[S7 connection_1]DB100X2.7"
#define ID_IMPRESION_REVERSE                "S7:[S7 connection_1]DB100X3.0"
#define ID_TENSAR_LAMINA                    "S7:[S7 connection_1]DB100X3.1"
#define ID_DESTENSAR_LAMINA                 "S7:[S7 connection_1]DB100X3.2"
#define ID_LAMPARA_VISION                   "S7:[S7 connection_1]DB100X3.3"
#define ID_LAMPARA_SETUP                    "S7:[S7 connection_1]DB100X3.4"
#define ID_LAMPARA_RESET                    "S7:[S7 connection_1]DB100X3.5"
#define ID_LAMPARA_JOGFORWARD               "S7:[S7 connection_1]DB100X3.6"
#define ID_LAMPARA_JOGREVERSE               "S7:[S7 connection_1]DB100X3.7"
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
#define ID_DIAMETRO_SALIDA_FILTERED         "S7:[S7 connection_1]DB100W42"
#define ID_NUMERO_MACULAS                   "S7:[S7 connection_1]DB100W44"
#define ID_LAMPARA_POS                      "S7:[S7 connection_1]DB100W46"
#define ID_FUERZA_CUCHILLAS                 "S7:[S7 connection_1]DB100W48"

//DB101
#define	ID_CONDICIONES_IMPRESION_EXT        "S7:[S7 connection_1]DB101X0.0" //Se lee a 1 cuando podamos imprimir
#define	ID_SECUENCIA_ON_EXT                 "S7:[S7 connection_1]DB101X0.1"
#define	ID_DETECTOR_MACULA_EXT              "S7:[S7 connection_1]DB101X0.2"
#define ID_DESPLAZANDO_FILM_EXT             "S7:[S7 connection_1]DB101X0.3"
#define ID_BUSCANDO_MACULA_EXT              "S7:[S7 connection_1]DB101X0.4"
#define ID_IMPRESION_EXT                    "S7:[S7 connection_1]DB101X0.5"
#define ID_BALLUF_CENTRADO                  "S7:[S7 connection_1]DB101X0.6"
#define ID_LAMINA_SIN_TENSION               "S7:[S7 connection_1]DB101X0.7"
#define ID_DESPLAZADOR_SET                 "S7:[S7 connection_1]DB101X1.0"
#define ID_DESPLAZADOR_INP                 "S7:[S7 connection_1]DB101X1.1"

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
#define ID_ERROR_BOBINADOR                              "S7:[S7 connection_1]DB101W14"
#define ID_ERROR_DESBOBINADOR                           "S7:[S7 connection_1]DB101W18"
#define ID_MAX_DISTANCIA_MACULA                         "S7:[S7 connection_1]DB101W28"
#define ID_MIN_DISTANCIA_MACULA                         "S7:[S7 connection_1]DB101W32"
#define ID_DIAM_PLC_OUTPUT                              "S7:[S7 connection_1]DB101W36"
#define ID_DIAM_PLC_INPUT                               "S7:[S7 connection_1]DB101W38"





/*1-Emergencia
 *2-Variador salida nok
 *3-Variador entrada nok
 *4-Fin trabajo
 *5-Fin bobina entrada
 *6-Fin bobina salida
 *7-Fallo arrastre
 *8-Bandeja nok
 *9-Mesa de impresión nok
 */

#endif

using namespace CemitecProxyOPC;

struct OPC_Item{
    int client_handle;
    QString ID;
    QString name;
};

struct OPC_Namespace{
    struct OPC_Item VelocidadActual;
    struct OPC_Item VelocidadRef;
    struct OPC_Item LongitudActual;
    struct OPC_Item LongitudRef;
    struct OPC_Item CondicionesImpresion;
    struct OPC_Item TrabajoNuevo;
    struct OPC_Item CondicionesMovimiento;
    struct OPC_Item DiametroActual;
    struct OPC_Item DiametroRef;
    struct OPC_Item CodigoParada;
    struct OPC_Item PermisoParar;
    struct OPC_Item DiametroParar;
    struct OPC_Item FijarDiametro;
};

/*struct OPC_Item_Ext1{
    int client_handle;
    QString ID;
    QString name;
    //QVariant valor;
};*/

struct OPC_NamespaceExt1{
    struct OPC_Item CondicionesMovimiento;
    struct OPC_Item CondicionesImpresion;
    struct OPC_Item ParoMaquina;
    struct OPC_Item MarchaMovimiento;
    struct OPC_Item ParoMovimiento;
    struct OPC_Item SecuenciaOn;
    struct OPC_Item Impresion;
    struct OPC_Item DetectorMacula;
    struct OPC_Item JogMas;
    struct OPC_Item JogMenos;
    struct OPC_Item BuscarMacula;
    struct OPC_Item BuscandoMacula;
    struct OPC_Item DeshabilitarServoEntrada;
    struct OPC_Item DeshabilitarServoSalida;
    struct OPC_Item DesplazarFilm;
    struct OPC_Item DesplazandoFilm;
    struct OPC_Item VelocidadActual;
    struct OPC_Item VelocidadRef;
    struct OPC_Item LongitudActual;
    struct OPC_Item LongitudRef;
    struct OPC_Item TrabajoNuevo;
    struct OPC_Item DiametroEntradaActual;
    struct OPC_Item DiametroEntradaRef;
    struct OPC_Item DiametroSalidaActual;
    struct OPC_Item DiametroSalidaRef;
    struct OPC_Item CodigoParada;
    struct OPC_Item PermisoParar;
    struct OPC_Item DiametroParar;
    struct OPC_Item FijarDiametroEntrada;
    struct OPC_Item FijarDiametroSalida;
    struct OPC_Item TiempoBusquedaMacula;
    struct OPC_Item VelocidadBusquedaMacula;
    struct OPC_Item VelocidadAjusteMacula;
    struct OPC_Item VelocidadJogMas;
    struct OPC_Item VelocidadJogMenos;
    struct OPC_Item FrenoElectromagneticoOnOff;
    struct OPC_Item FuerzaFrenoElectromagnetico;
    struct OPC_Item IntensidadLampara;
    struct OPC_Item TemperaturaLampara;
    struct OPC_Item DistanciaEmpalme;
    struct OPC_Item TensionBobina;
    struct OPC_Item RampaAceleracion;
    struct OPC_Item RampaDeceleracion;
    struct OPC_Item LamparaOn;
    struct OPC_Item ApagarXUSB1;
    struct OPC_Item ApagarXUSB2;
    struct OPC_Item EnableCalculoDiametro;
    struct OPC_Item DiametroEntradaFiltered;
    struct OPC_Item Impresion_Reverse;
    struct OPC_Item Tensar_Lamina;
    struct OPC_Item Destensar_Lamina;
    struct OPC_Item LamparaVision;
    struct OPC_Item Balluf_Centrado;
    struct OPC_Item Lamina_sin_tension;
    struct OPC_Item Error_Bobinador;
    struct OPC_Item Error_DesBobinador;
    struct OPC_Item NumeroMaculas;
    struct OPC_Item DistanciaMaculaMax;
    struct OPC_Item DistanciaMaculaMin;
    struct OPC_Item DiametroSalidaFiltered;    
    struct OPC_Item LamparaMover;
    struct OPC_Item LamparaSetUp;
    struct OPC_Item LamparaReset;
    struct OPC_Item LamparaJogF;
    struct OPC_Item LamparaJogR;
    struct OPC_Item LamparaPos;
    struct OPC_Item DesplzSet;
    struct OPC_Item DesplzInp;
    struct OPC_Item DiamPLC_Output;
    struct OPC_Item DiamPLC_Input;
    struct OPC_Item BajarCuchillas;
    struct OPC_Item FuerzaCuchillas;
};


class ClienteOPC : public QObject
{
    Q_OBJECT

public:
    static ClienteOPC *request(QObject *parent);
    //Constructor
    void timedEscribeDato(int ReferenciaItem,QVariant valor);
    void timedExtEscribeDato(int delay,int ReferenciaItem,QVariant valor);
    void Init();
    void setEnabled(bool val);
    QVariant returnOPCHandle(int ReferenciaItem);
    bool initiated();
    ~ClienteOPC();

public:
    struct OPC_NamespaceExt1 *OPC_Bobinadora_Ext1;
    struct OPC_Namespace *OPC_Bobinadora;
    CemitecObjectOPC *MiServidorOPC;

private:

    explicit ClienteOPC(QObject *parent = 0);
    //Añadimos un nuevo item
    void setNewItem(struct OPC_Item *p,QString variable,QString name);
    struct OPC_Item * ReturnOPCItem(int ReferenciaItem);

private:
    int contador_client_handle;
     QVector<int> m_PendingItems;
     QVector<QVariant> m_PendingValues;
    static ClienteOPC *mInstance;
    bool m_Monitor;
    bool m_enabled;
    MemoryMap *m_memoryMap;
    bool m_bPLCSensorDiameterIN;
    bool m_bPLCSensorDiameterOUT;
    bool m_bInitiated;


public slots:
    void InicializarItems();
    void InicializarItemsExt1();
    void InicializarItemsExt1Advanced();
    void EscribirDato(int ReferenciaItem,QVariant valor);
    QVariant LeerDato(int ReferenciaItem);
    void debugDLL(int,QString,QString,QString);
    void sltEnableMonitoring(bool val);
    void desconectaServidor();
    void sltUpdateVelocidad();
    void sltMessageFromLampara(int ReferenciaItem,QVariant valor);
    void sltMessageFromCorte(int ReferenciaItem,QVariant valor);

private slots:
    void sltUpdatePendingValue();
    void sltMonitoring();
    void sltMonitoringLong();
    void slTTestThreadID();

signals:
    void SendDebugInfo(unsigned char code, QString msg);
    void sgnOPCFlanco(int ID);
    void sgnOPCBallufChange();
    void OPCInitiated();
    void sgnStartOPCMonitor();
    void sgnOPCNewLamparaValue(int ReferenciaItem,QVariant value);
};

#endif // CLIENTEOPC_H
