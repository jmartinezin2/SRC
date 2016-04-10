#include "truco.h"
#include <iostream>
#include <windows.h>
//#include <winable.h> /* Dev-C++ specific */
#include "globals.h"
#include <QTime>
#include <QKeyEvent>

using namespace std;

HWND GameWindow;
HDESK hDesk;
LPSTR title;

void NombreVentana(){
	
	/*
	HWND    GameWindow = FindWindowA(NULL,"XUSB Windows Application - xaar 1001 testwedge8l - (1000 Channels, 3556 Strokes)");
	SetForegroundWindow(GameWindow);
	*/

    EnumWindows(MyEnumProc, 0);

}

BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam)
{
    //char wn_l[5]="XUSB";
    char wn_l[3]="XU";
    //bool encontrada;
	TCHAR wn[5];

    //encontrada=TRUE;

	if (!hWnd) return TRUE; // Not a window
	else if (!::IsWindowVisible(hWnd))return TRUE;		// Not visible
	else{
        //iLen = GetWindowText(hWnd, wn, 5);
        GetWindowText(hWnd, wn, 3);
		if ( (wn[0]==((TCHAR)wn_l[0])) && 
            (wn[1]==((TCHAR)wn_l[1])) /*&&
            (wn[2]==((TCHAR)wn_l[2])) &&
            (wn[3]==((TCHAR)wn_l[3])) &&
            (wn[4]==((TCHAR)wn_l[4]))*/){
                    ShowWindow(hWnd,SW_RESTORE);
                    SetForegroundWindow(hWnd);
                    //SetForegroundWindow function
                    //If the window was brought to the foreground, the return value is nonzero.
                    //If the window was not brought to the foreground, the return value is zero.

                    //if ((codigo_error==1) || (codigo_error==100) || (codigo_error==101)) GenerateKey('R', FALSE); //Stop
                    //else GenerateKey('P', FALSE);   //Soft-stop

                    GenerateKey('P', FALSE);   //Soft-stop


                    //encontrada=true;
                    return FALSE;
		}
        else{
            return TRUE;
        }
	}

    //if (encontrada==true)return FALSE;
    //else return TRUE;

}


void HandleXUSBVentanaPrincipal(){

    EnumWindows(FindXUSBVentanaPrincipal, 0);

}

BOOL CALLBACK FindXUSBVentanaPrincipal(HWND hWnd, LPARAM lParam)
{
    char wn_l[3]="XU";
    TCHAR wn[3];
    //BOOL aux;

    if (!hWnd) return TRUE; // Not a window
    else if (!::IsWindowVisible(hWnd))return TRUE;		// Not visible
    else{
        GetWindowText(hWnd, wn, 3);
        if ( (wn[0]==((TCHAR)wn_l[0])) &&
            (wn[1]==((TCHAR)wn_l[1])) ){
            HandleXUSB=hWnd;
            return FALSE; //NO PONERLO
        }
        else{
             return TRUE;
        }
    }
}


void HandleXUSBVentanaPrintComplete(){

    NumeroVentanas=0;
    EnumWindows(FindXUSBVentanaPrintComplete, 0);

}

BOOL CALLBACK FindXUSBVentanaPrintComplete(HWND hWnd, LPARAM lParam)
{
    char wn_l[3]="XU";
    TCHAR wn[3];

    //BOOL aux;

    if (!hWnd) return TRUE; // Not a window
    else if (!::IsWindowVisible(hWnd))return TRUE;		// Not visible
    else{
        GetWindowText(hWnd, wn, 3);
        if ( (wn[0]==((TCHAR)wn_l[0])) &&
            (wn[1]==((TCHAR)wn_l[1])) ){
            NumeroVentanas++;
            return FALSE; //NO PONERLO
        }
        else{
             return TRUE;
        }
    }

}




/* This is a function to simplify usage of sending keys */
void GenerateKey(int vk, BOOL bExtended) {

    KEYBDINPUT  kb = {0};
    INPUT       Input = {0};

    /* Generate a "key down" */
    if (bExtended) { kb.dwFlags  = KEYEVENTF_EXTENDEDKEY; }
    kb.wVk  = vk;
    Input.type  = INPUT_KEYBOARD;
    Input.ki  = kb;
    SendInput(1, &Input, sizeof(Input));

    /* Generate a "key up" */
    ZeroMemory(&kb, sizeof(KEYBDINPUT));
    ZeroMemory(&Input, sizeof(INPUT));
    kb.dwFlags  =  KEYEVENTF_KEYUP;
    if (bExtended) { kb.dwFlags |= KEYEVENTF_EXTENDEDKEY; }
    kb.wVk = vk;
    Input.type = INPUT_KEYBOARD;
    Input.ki = kb;
    SendInput(1, &Input, sizeof(Input));

    return;
}

