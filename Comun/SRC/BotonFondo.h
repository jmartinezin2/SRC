
#ifndef _BOTONFONDO_H_
#define _BOTONFONDO_H_

#include <QPushButton>

/**
 * @brief Clase que pinta un boton con una imagen de fondo.
 *
 * Esta clase derivada de QPushButton pinta un boton con una imagen de fondo.
 * La imagen es escalada para ajustarla al botón. Además se pueden ajustar
 * unos márgenes no escalables de forma que sólo se escale la zona central.
 * Además utiliza dos configuraciones diferentes dependiendo de si el botón
 * está normal o si está hundido o seleccionado.
 * Características:
 * @li Deriva de QPushButton.
 * @li Permite ajustar unos márgenes de escalado para no deformar la imagen.
 * @li Permite utilizar una imagen para el estado normal y otra para cuando el
 * botón está hundido o seleccionado.
 * @li Utiliza el color QPalette::ButtonText para el texto del botón en estado
 * normal y el color QPalette::BrightText para el texto del botón cuando está
 * hundido o seleccionado.
 */
class BotonFondo : public QPushButton
{
     Q_OBJECT
     Q_PROPERTY(uint     superior  READ superior  WRITE setSuperior)
     Q_PROPERTY(uint     inferior  READ inferior  WRITE setInferior)
     Q_PROPERTY(uint     derecho   READ derecho   WRITE setDerecho)
     Q_PROPERTY(uint     izquierdo READ izquierdo WRITE setIzquierdo)
     Q_PROPERTY(QPixmap  fondo        READ fondo        WRITE setFondo)
     Q_PROPERTY(QPixmap  seleccionado READ seleccionado WRITE setSeleccionado)
     Q_PROPERTY(Qt::Alignment alineacion READ alineacion WRITE setAlineacion)
     Q_PROPERTY(Qt::Alignment alineacionIcono READ alineacionIcono WRITE setAlineacionIcono)

public:
     BotonFondo(QWidget *parent = 0);
     ~BotonFondo();

     uint superior() const;
     void setSuperior(const uint uiSuperior);
     uint inferior() const;
     void setInferior(const uint uiInferior);
     uint derecho() const;
     void setDerecho(const uint uiDerecho);
     uint izquierdo() const;
     void setIzquierdo(const uint uiIzquierdo);
     void setMargenes(const uint uiSuperior, const uint uiInferior, const uint uiDerecho, const uint uiIzquierdo);

     QPixmap   fondo() const;
     void      setFondo(const QPixmap & qpixFondo);
     QPixmap   seleccionado() const;
     void      setSeleccionado(const QPixmap & qpixSeleccionado);

     Qt::Alignment  alineacion() const;
     void           setAlineacion(const Qt::Alignment qalgnAlineacion);
     Qt::Alignment  alineacionIcono() const;
     void           setAlineacionIcono(const Qt::Alignment qalgnAlineacion);
	     QSize     sizeHint() const;

private:
     void paintEvent(QPaintEvent * event);
     void resizeEvent(QResizeEvent * event);

     void ajustarFondos();
     void ajustarFondo(const QPixmap & qpixOriginal, QPixmap & qpixAjustado);

     /// Mantiene la imagen de fondo original.
     QPixmap        m_qpixFondoOriginal;
     /// Mantiene la imagen de fondo escalada al tamaño actual.
     QPixmap        m_qpixFondo;
     /// Mantiene la imagen de seleccionado original.
     QPixmap        m_qpixSeleccionadoOriginal;
     /// Mantiene la imagen de seleccionado escalada al tamaño actual.
     QPixmap        m_qpixSeleccionado;

     /// Manitene el margen superior.
     uint           m_uintSuperior;
     /// Manitene el margen inferior.
     uint           m_uintInferior;
     /// Manitene el margen derecho.
     uint           m_uintDerecho;
     /// Manitene el margen izquierdo.
     uint           m_uintIzquierdo;

     /// Manitene la alineación del texto.
     Qt::Alignment  m_qalgnAlineacion;
     /// Manitene la alineación del icono.
     Qt::Alignment  m_qalgnAlineacionIcono;

};

#endif /* _BOTONFONDO_H_ */

/** @}*/
