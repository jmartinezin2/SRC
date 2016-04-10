#include "gestorfluidica.h"

GestorFluidica::GestorFluidica(QObject *parent) :
    QObject(parent)
{
    m_units=0;
    XCL_Initialise(&m_units);

}
