//****************************************************************************
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (C) 2009  XaarJet Ltd.  All Rights Reserved.
// Description: XCL Library
// Author: JON
// Version: 1.1
//****************************************************************************

#ifndef _XCLIF_HEADER
#define _XCLIF_HEADER

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef XCL_EXPORTS
#define XCL_API __declspec(dllexport) XCL_Errors __stdcall
#include "XCOMMS.h"
#else
#define XCL_API __declspec(dllimport) XCL_Errors __stdcall
#endif

#define XCL_USB_IDENT "HYDRA ISS"

#define XCL_MAX_CTRLS   32      /* Max number of controllers */
#define XCL_MAX_SERIAL  32      /* Max number of characters in serial number string */
#define XCL_MAX_DESCR   128     /* Max number of characters in description string */
#define XCL_MAX_BLOCK   399     /* 399 user FLASH blocks, numbered 1 to 399 */

#define XCL_LED_Warning 0x010   /* Bit masks for LEDs, Switches and Fault codes */
#define XCL_LED_Ready   0x020
#define XCL_LED_Fault   0x040
#define XCL_LED_Spare   0x080
#define XCL_LED_Heat    0x100
#define XCL_LED_Valve   0x200

#define XCL_SW_Supp1    0x001
#define XCL_SW_Supp2    0x002
#define XCL_SW_Ret1     0x004
#define XCL_SW_Ret2     0x008
#define XCL_SW_Start    0x010
#define XCL_SW_Stop     0x020
#define XCL_SW_Shutdn   0x040
#define XCL_SW_Fill     0x080
#define XCL_SW_UPS      0x100

#define XCL_FLT_Sens            0x000F  /* Mask for last sensor in error */
#define XCL_FLT_FLASH           0x0010  /* FLASH error */
#define XCL_FLT_NOIMP1          0x0020  /* not implemented*/
#define XCL_FLT_TACHO_BOUNDS    0x0040  /* Tacho bounds exceeded. */
#define XCL_FLT_ESTOP           0x0080  /* Stop commanded */
#define XCL_FLT_OVER_TEMP       0x0200  /* Over temperature */
#define XCL_FLT_NOIMP2          0x1000  /* not implemented*/
#define XCL_FLT_NO_INK          0x2000  /* Ink errors */
#define XCL_FLT_XS_INK          0x4000
#define XCL_FLT_PUMP_STALL      0x8000  /* Pump stalled during operating cycle */

typedef enum {                  /* Command codes */
    XCL_CMD_Reboot,		/* Reboot the Hydra unit */
    XCL_CMD_Start,		/* Start the Hydra unit */
    XCL_CMD_Stop,		/* Stop the Hydra unit */
    XCL_CMD_Idle,		/* reserved for write, reflects that the unit is in idle mode when read. */
    XCL_CMD_Shutdown,		/* Shutdown the Hydra unit */
    XCL_CMD_Debug,		/* reserved */
    XCL_NUM_COMMANDS
} XCL_Commands;

typedef enum {                  /* Controller states */
    XCL_MODE_Stopped,
    XCL_MODE_Filling,
    XCL_MODE_Starting_MP,
    XCL_MODE_Starting_DP,
    XCL_MODE_Starting_T,
    XCL_MODE_Started,
    XCL_MODE_Idling,
    XCL_MODE_Stopping_T,
    XCL_MODE_Stopping_DP,
    XCL_MODE_Stopping_MP,
    XCL_MODE_Venting,
    XCL_NUM_MODES
} XCL_Modes;

//static const char *XCL_Modes_Description[XCL_NUM_MODES] = {"Stopped","Filling","Starting: Meniscus Pressure Setpoint","Starting: Differential Pressure Setpoint","Starting: Temperature Setpoint","Started", "Idling", "Stopping: Temperature Setpoint", "Stopping: Differential Pressure Setpoint", "Stopping: Meniscus Setpoint","Venting to air"};


typedef enum {                  /* Function return error codes */
    XCL_NO_ERROR,
    XCL_ERR_NOT_INITIALISED,
    XCL_ERR_CONTROLLER_ID,
    XCL_ERR_NO_CONTROLLER,
    XCL_ERR_CONTROLLER_LOST,
    XCL_ERR_BAD_DATA,
    XCL_ERR_TIMEOUT,
    XCL_ERR_SENSOR_ID,
    XCL_ERR_DRIVE_ID,
    XCL_ERR_PID_ID,
    XCL_ERR_FILTER_ID,
    XCL_ERR_BLOCK_NUM,
    XCL_ERR_COMMAND,
    XCL_ERR_BOUNDS,
    XCL_NUM_ERRORS
} XCL_Errors;

//static const char *XCL_Errors_Description[] = {"No Error","Not Initialised","Invalid Controller ID",
//"No Controller","Controller Lost","Bad Data","Link Timeout","Invalid Sensor ID","Invalid Drive ID",
//"Invalid Controller ID","Invalid Filter ID","Invalid Flash Block Number","Invalid Command","Data Out of Bounds"};

typedef enum {                  /* Filter characteristics */
    XCL_FLT_OFF,
    XCL_FLT_010,
    XCL_FLT_020,
    XCL_FLT_050,
    XCL_FLT_100,
    XCL_FLT_200,
    XCL_FLT_500,
    XCL_NUM_FILTERS
} XCL_Filters;

typedef enum {                  /* Sensor (ADC) ID values */
    XCL_SENS_Supply_Pr,
    XCL_SENS_Return_Pr,
    XCL_SENS_Vaccuum_Pr,
    XCL_SENS_Pressure_Pr,
    XCL_SENS_Level_Res,
    XCL_SENS_Supply_Temp,
    XCL_SENS_Return_Temp,
    XCL_SENS_Res_Temp,
    XCL_NUM_NO_Ambient_SENSORS,	//XCL_SENS_Ambient_Temp,
    XCL_NUM_SENSORS
} XCL_Sensors;

//const 
//static const char *XCL_Sensors_Description[] = {"Supply Pressure Sensor","Return Pressure Sensor","","","Reservoir Level Sensor","Supply Temperature Sensor","Return Temperature Sensor","Reservoir Temperature", ""};//,"Ambient Temperature"};

typedef enum {                  /* Drive (DAC) ID values */
    XCL_DRV_Fill,
    XCL_DRV_Supply,
    XCL_DRV_Return,
    XCL_DRV_Vaccuum,
    XCL_DRV_Pressure,
    XCL_DRV_Heater,
    XCL_NUM_DRIVES
} XCL_Drives;

//static const char *XCL_Drives_Description[] = {"Fill Pump Drive","Supply Pump Drive","Return Pump Drive","","","In Line Heater Drive"};

typedef enum {                  /* Control loop (PID) ID values */
    XCL_PID_Temp,
    XCL_PID_Press,
    XCL_PID_Vac,
    XCL_NUM_PIDS
} XCL_Controls;

//static const char *XCL_Controls_Description[] = {"Temperature PID","Differential Pressure PID","Meniscus Pressure PID"};


typedef enum {                  /* MSP Integer Coefficient identities  */
    MSP_NUM_ICOEFFS_ACT,
    MSP_LEV_EMPTY,
    MSP_LEV_LOW,
    MSP_LEV_NORM,
    MSP_LEV_TOP,
    MSP_LEV_FILL,
    MSP_LEV_MAX,
    MSP_TIMER1,
    MSP_TIMER2,
    MSP_TIMER3,
    MSP_PID_MODE,
    MSP_TIMEOUT,
    MSP_FILL_FAULT,
    MSP_COLOUR,
	MSP_RATIO_TIM,
    MSP_NUM_IC
} XCL_Icoeffs;


//static const char *XCL_Icoeffs_Description[] = {"","Reservoir Level: Empty","Reservoir Threshold: Low","Reservoir Threshold: Normal","Reservoir Threshold: Top","Reservoir Threshold: Fill","Reservoir Threshold: Maximum","Dwell Time: Temperature","Dwell Time: Pressure","Dwell Time: Purge","PID Mode","Fault Time","Fill Pump Timeout","Ink Colour","Pump Ratio Timeout"};
//static const char *XCL_Icoeffs_Dimensions[] = {"None","None","None","None","None","None","None","Seconds","Seconds","Seconds","None","Seconds","Seconds","None","Seconds"};
//static const short XCL_Icoeffs_LowClip[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//static const short XCL_Icoeffs_UpperClip[] = {0,12,12,12,12,12,12,300,300,300,1,300,300,32767,300};

typedef enum {                  /* MSP Floating Point Coefficient identities */
    MSP_NUM_FCOEFFS_ACT,
    MSP_PC_THIRTY,
    MSP_FILL_SLOW,
    MSP_FILL_FAST,
    MSP_PC_NINETY,
    MSP_HEAD1,
    MSP_HEAD2,
	MSP_RATIO_MAX,
	MSP_RATIO_MIN,
    MSP_NUM_FC
} XCL_Fcoeffs;

//static const char *XCL_Fcoeffs_Description[] = {"","Initial Pump","Reservoir Slow Fill Voltage","Reservoir Fast Fill Voltage","Purge Pump","Supply Datum Pressure Head Offset","Return Datum Pressure Head Offset","Supply / Return Pump Ratio Max","Supply / Return Pump Ratio Min"};
//static const char *XCL_Fcoeffs_Dimensions[] = {"None","Volts","Volts","Volts","Volts","mBar","mBar","Dimensionless","Dimensionless"};
//static const float XCL_Fcoeffs_LowClip[] = {0,0,0,0,0,-1000,-1000,0,0};
//static const float XCL_Fcoeffs_UpperClip[] = {0,5,5,5,5,1000,1000,10,10};


typedef struct {                /* Holds ADC (sensor) information */
    char reserved1;
    char reserved2;
    short reserved3;
    short reserved4;
    short offs;                   /* Offset of channel to 0 */
    short xn;                     /* Raw ADC value (offsetted) */
    short yn;                     /* Filtered value */
    short cmax;                   /* Clip/fault level for upper value */
    short cmin;                   /* Clip/fault level for lower value */
    float scale;               /* Calibration/scale factor of channel */
    float val;                 /* The processed value of the channel */
} XCL_ADC;

typedef struct {                /* Holds DAC (drive) information */
    char reserved1;
    char reserved2;
    short tacho1;                 /* Measured pump1 speed */
    short tacho2;                 /* Measured pump2 speed */
    short cmax;                   /* Clip level for upper value (no warning) */
    short pwm;                    /* PWM counter value */
    short patt;                   /* Converted output */
    float scale;               /* Scale factor to apply to convert to short */
    float val;                 /* The real value to output */
} XCL_DAC;

typedef struct {                /* Holds PID control loop information */
    short ch1;                    /* First sensor input ch  } Takes ch1 + ch2 - ch3 */
    short ch2;                    /* Second sensor input ch }  as the PID i/p */
    short ch3;                    /* Third sensor input ch  } */
    short opCH;                   /* Output channel select */
    short countd;                 /* Deemed stable when counts down to 0 */
    float stabTim;             /* Time for which setpoint must be stable */
    float posn;                /* Current position */
    float setp1;               /* Setpoint for normal operation */
    float setp2;               /* Alternative setpoint */
    float err;                 /* Current error */
    float perr;                /* Previous error */
    float tol;                 /* Setpoint tolerance */
    float integ;               /* The integration term */
    float Kp;                  /* The PID coeffs */
    float Ki;
    float Kd;
    float cmax;                /* Clip levels for all values */
    float cmin;
    float dem;                 /* New drive demand */
} XCL_PID;

// Structures from version 1.1.X of the DLL

typedef enum{
	XCL_BOOTINITIALISATION, // The bootloader is initialising.
	XCL_FALLBACK, // The bootloader fallback mode has been entered.
	XCL_APPLICATION  // The application is running post boot.
}XCL_BOOT_MODES;

// Data Structures.

typedef struct{
  unsigned short VersionMajor; // Version of the structure.
  unsigned short VersionMinor; // Version of the structure.
  unsigned short HardwareRevStart; // Hardware Revision Level, starting from.
  unsigned short HardwareRevStop;  // Hardware Revision Level, stopping at.

}XCL_Application_Header;


typedef struct{
  unsigned short VersionMajor; // Version of the Hydra unit major.
  unsigned short VersionMinor; // Version of the Hydra unit minor.
  unsigned short HardwareRev; // Hardware Revision Level fo the Hydra.
  unsigned short XaarPN; // The Xaar part number of the Hydra.
  unsigned short Mode; // Current mode of the bootstrap.
}
XCL_Boot_Header; // Once the application has booted, this structure resides in a fixed place in flash store.


// Functions from version 1.0.X of the DLL.

XCL_API XCL_Initialise (short *Found); 
XCL_API XCL_Close(short ID);
XCL_API XCL_Restart(short ID);
XCL_API XCL_Read_HYDRA_Description(short ID, char *Description,short Length);
XCL_API XCL_Write_HYDRA_Description(short ID, char *Description,short Length);
XCL_API XCL_Get_Info (short ID, char SerialNum [], char Length, short *CurrentBootBlock);
XCL_API XCL_Get_Block_Desc (short ID, char Descrip [],short Length, short DescBlock);
XCL_API XCL_Read_Sensor (short ID, XCL_Sensors Select, XCL_ADC *Details);
XCL_API XCL_Write_Sensor (short ID, XCL_Sensors Select, XCL_ADC *Details);
XCL_API XCL_Read_Filter (short ID, XCL_Sensors Select, XCL_Filters *TimeConstant);
XCL_API XCL_Write_Filter (short ID, XCL_Sensors Select, XCL_Filters *TimeConstant);
XCL_API XCL_Read_Drive (short ID, XCL_Drives Select, XCL_DAC *Details);
XCL_API XCL_Write_Drive (short ID, XCL_Drives Select, XCL_DAC *Details);
XCL_API XCL_Read_Control (short ID, XCL_Controls Select, XCL_PID *Details);
XCL_API XCL_Write_Control (short ID, XCL_Controls Select, XCL_PID *Details);
XCL_API XCL_Write_Icoeff (short ID, short *Iarray);
XCL_API XCL_Read_Icoeff (short ID, short *Iarray);
XCL_API XCL_Write_Fcoeff (short ID, float *Farray);
XCL_API XCL_Read_Fcoeff (short ID, float *Farray);
XCL_API XCL_Read_Log (short ID, float *Farray);
XCL_API XCL_Read_Switches (short ID, short *Bits);
XCL_API XCL_Read_LEDs (short ID, short *Bits, short *Fault);
XCL_API XCL_Read_Mode (short ID, XCL_Modes *Mode);
XCL_API XCL_Send_Command (short ID, XCL_Commands Command);
XCL_API XCL_Save_To_FLASH (short ID, short BlockToWrite, short NewBootBlock, char *NewDescrip,short DescSize);
XCL_API XCL_Save_To_Disk(short ID,LPCWSTR Filename,LPCWSTR Description,short DescSize);
XCL_API XCL_Load_From_Disk(short ID,LPCWSTR Filename,LPCWSTR Description,short DescSize);
XCL_API XCL_Colour_Remap(int *Uncompressed, short *Compressed,bool Compress);


// Functions from version 1.1.X of the DLL.

XCL_API XCL_Write_Firmware(short ID, unsigned char *newFirmware,unsigned short *Progress, unsigned short Length);
XCL_API XCL_Read_Boot_Header(short ID,XCL_Boot_Header *bootHead);
XCL_API XCL_Read_Application_Header(short ID,XCL_Application_Header *appHead);

#ifdef __cplusplus
};
#endif

#endif

