
#include <windows.h>
//#include <winable.h> /* Dev-C++ specific */

void GenerateKey(int vk, BOOL bExtended);

void NombreVentana();
BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam);

void HandleXUSBVentanaPrincipal();
BOOL CALLBACK FindXUSBVentanaPrincipal(HWND hWnd, LPARAM lParam);

void HandleXUSBVentanaPrintComplete();
BOOL CALLBACK FindXUSBVentanaPrintComplete(HWND hWnd, LPARAM lParam);

