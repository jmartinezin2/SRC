// dibapi.h
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#ifndef _INC_DIBAPI
#define _INC_DIBAPI

/* Handle to a DIB */
DECLARE_HANDLE(HDIB);

/* DIB constants */
#define PALVERSION   0x300

/* DIB Macros*/

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)


/* Function prototypes */
BOOL    WINAPI  PaintDIB (HDC, LPRECT, HDIB, LPRECT, CPalette* pPal);
BOOL    WINAPI  CreateDIBPalette(HDIB hDIB, CPalette* cPal);
LPSTR   WINAPI  FindDIBBits (LPSTR lpbi);
DWORD   WINAPI  DIBWidth (LPSTR lpDIB);
DWORD   WINAPI  DIBHeight (LPSTR lpDIB);
DWORD	WINAPI	DIBSize (LPSTR lpDIB);
WORD    WINAPI  PaletteSize (LPSTR lpbi);
WORD    WINAPI  DIBNumColors (LPSTR lpbi);
HGLOBAL WINAPI  CopyHandle (HGLOBAL h);

HDIB    WINAPI  ReadDIBFile(CFile& file);
HDIB	WINAPI	ReadDIBFile(FILE *fStream);
BOOL    WINAPI  SaveDIB (HDIB hDib, CFile& file);

//Angle bitmap for Printhead XJ126
HDIB	WINAPI CopyAngleSwathesDIBXJ126(HDIB hDIB, float fAngle, BOOL bFirstBChannel, BOOL bFixedTop, bool RemapFlag, WORD wNumColours, DWORD dwBackColour, DWORD dwMaxGreyLevels);
HDIB	WINAPI GetDIBSwatheXJ126(HDIB hDIB, DWORD dwSwatheNumber, WORD wNumColours);
HDIB	WINAPI CopyAngleSourceDIBXJ126(HDIB hDIB, float fAngle, BOOL bFirstBChannel, BOOL bFixedTop, bool RemapFlag, WORD wNumColours, DWORD dwBackColour, DWORD dwMaxGreyLevels);

// Special swathe loading mode
HDIB WINAPI ReadSwatheFile(CFile& file, LPCTSTR FileName);
HDIB WINAPI ReadSwatheFileUpdated(CFile& file, LPCTSTR FileName);
 
#endif //!_INC_DIBAPI
