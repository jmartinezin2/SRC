/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <QPainter>
#include <QStylePainter>
#include <QApplication>
#include <QStyleOptionButton>
#include "BotonFondo.h"

/**
 * @brief Constructor de la clase.
 *
 * @param [in] parent         Ventana padre.
 *
 * Constructor de la clase.
 * Ajusta el valor inicial de las variables miembro y calcula el fondo en el estado inicial.
 */
BotonFondo::BotonFondo(QWidget *parent)
     : QPushButton(parent), m_uintSuperior(0), m_uintInferior(0), m_uintDerecho(0), m_uintIzquierdo(0),
     m_qalgnAlineacion(Qt::AlignHCenter), m_qalgnAlineacionIcono(Qt::AlignVCenter)
{
     this->ajustarFondos();
}

/**
 * @brief Destructor de la clase.
 */
BotonFondo::~BotonFondo()
{
}

////////////// PROPIEDADES ///////////////////////
/**
 * @brief Indica el valor actual del margen superior.
 *
 * @return          El valor actual del margen superior.
 */
uint BotonFondo::superior() const
{
     return m_uintSuperior;
}

/**
 * @brief Ajusta el valor del margen superior.
 *
 * @param [in] uiSuperior          Nuevo valor del margen superior.
 */
void BotonFondo::setSuperior(const uint uiSuperior)
{
     if (m_uintSuperior != uiSuperior)
     {
          m_uintSuperior = uiSuperior;
          this->ajustarFondos();
     }
}

/**
 * @brief Indica el valor actual del margen inferior.
 *
 * @return          El valor actual del margen inferior.
 */
uint BotonFondo::inferior() const
{
     return m_uintInferior;
}

/**
 * @brief Ajusta el valor del margen inferior.
 *
 * @param [in] uiInferior          Nuevo valor del margen inferior.
 */
void BotonFondo::setInferior(const uint uiInferior)
{
     if (m_uintInferior != uiInferior)
     {
          m_uintInferior = uiInferior;
          this->ajustarFondos();
     }
}

/**
 * @brief Indica el valor actual del margen derecho.
 *
 * @return          El valor actual del margen derecho.
 */
uint BotonFondo::derecho() const
{
     return m_uintDerecho;
}

/**
 * @brief Ajusta el valor del margen derecho.
 *
 * @param [in] uiDerecho           Nuevo valor del margen derecho.
 */
void BotonFondo::setDerecho(const uint uiDerecho)
{
     if (m_uintDerecho != uiDerecho)
     {
          m_uintDerecho = uiDerecho;
          this->ajustarFondos();
     }
}

/**
 * @brief Indica el valor actual del margen izquierdo.
 *
 * @return          El valor actual del margen izquierdo.
 */
uint BotonFondo::izquierdo() const
{
     return m_uintIzquierdo;
}

/**
 * @brief Ajusta el valor del margen izquierdo.
 *
 * @param [in] uiIzquierdo         Nuevo valor del margen izquierdo.
 */
void BotonFondo::setIzquierdo(const uint uiIzquierdo)
{
     if (m_uintIzquierdo != uiIzquierdo)
     {
          m_uintIzquierdo = uiIzquierdo;
          this->ajustarFondos();
     }
}

/**
 * @brief Ajusta los valores de los cuatro márgenes.
 *
 * @param [in] uiSuperior          Nuevo valor del margen superior.
 * @param [in] uiInferior          Nuevo valor del margen inferior.
 * @param [in] uiDerecho           Nuevo valor del margen derecho.
 * @param [in] uiIzquierdo         Nuevo valor del margen izquierdo.
 */
void BotonFondo::setMargenes(const uint uiSuperior, const uint uiInferior, const uint uiDerecho, const uint uiIzquierdo)
{
     m_uintSuperior = uiSuperior;
     m_uintInferior = uiInferior;
     m_uintIzquierdo = uiIzquierdo;
     m_uintDerecho = uiDerecho;

     this->ajustarFondos();
}

/**
 * @brief Indica la imagen de fondo utilizada actualmente.
 *
 * @return          La imagen de fondo utilizada actualmente.
 */
QPixmap BotonFondo::fondo() const
{
     return m_qpixFondoOriginal;
}

/**
 * @brief Fija el nuevo fondo de botón a utilizar.
 *
 * @param [in] qpixFondo      El nuevo fondo de botón a utilizar.
 */
void BotonFondo::setFondo(const QPixmap & qpixFondo)
{
     m_qpixFondoOriginal = qpixFondo;
     this->ajustarFondo(m_qpixFondoOriginal, m_qpixFondo);
}

/**
 * @brief Indica la imagen de fondo utilizada actualmente en modo seleccionado.
 *
 * @return          La imagen de fondo utilizada actualmente en modo seleccionado.
 */
QPixmap BotonFondo::seleccionado() const
{
     return m_qpixSeleccionadoOriginal;
}

/**
 * @brief Fija el nuevo fondo de botón a utilizar en modo seleccionado.
 *
 * @param [in] qpixSeleccionado         El nuevo fondo de botón a utilizar en modo seleccionado.
 */
void BotonFondo::setSeleccionado(const QPixmap & qpixSeleccionado)
{
     m_qpixSeleccionadoOriginal = qpixSeleccionado;
     this->ajustarFondo(m_qpixSeleccionadoOriginal, m_qpixSeleccionado);
}

/**
 * @brief Indica la alineación que está activa actualmente.
 *
 * @return          La alineación que está activa actualmente.
 */
Qt::Alignment BotonFondo::alineacion() const
{
     return m_qalgnAlineacion;
}

/**
 * @brief Fija la nueva alineación a utilizar.
 *
 * @param [in] qalgnAlineacion          La nueva alineación a utilizar.
 */
void BotonFondo::setAlineacion(const Qt::Alignment qalgnAlineacion)
{
     if (m_qalgnAlineacion != qalgnAlineacion)
     {
          m_qalgnAlineacion = qalgnAlineacion;
          this->update();
     }
}

/**
 * @brief Indica la alineación de icono que está activa actualmente.
 *
 * @return          La alineación de icono que está activa actualmente.
 */
Qt::Alignment BotonFondo::alineacionIcono() const
{
     return m_qalgnAlineacionIcono;
}

/**
 * @brief Fija la nueva alineación de icono a utilizar.
 *
 * @param [in] qalgnAlineacion          La nueva alineación de icono a utilizar.
 */
void BotonFondo::setAlineacionIcono(const Qt::Alignment qalgnAlineacion)
{
     if (m_qalgnAlineacionIcono != qalgnAlineacion)
     {
          m_qalgnAlineacionIcono = qalgnAlineacion;
          this->update();
     }
}

/**
 * @brief Ajusta los fondos del botón.
 *
 * Ajusta los fondos del botón.
 * Esta función se llama cada vez que cambia alguno de los márgenes.
 * Llama a la función de ajustar fondo para cada uno de los fondos
 * disponibles.
 */
void BotonFondo::ajustarFondos()
{
     this->ajustarFondo(m_qpixFondoOriginal, m_qpixFondo);
     this->ajustarFondo(m_qpixSeleccionadoOriginal, m_qpixSeleccionado);
}

/**
 * @brief Ajusta el fondo del botón solicitado.
 *
 * @param [in] qpixOriginal        Imagen del fondo original.
 * @param [in] qpixAjustado        Imagen del fondo ajustado.
 *
 * Ajusta el fondo del botón.
 * Esta función se llama cada vez que cambia alguna imagen de fondo
 * o cuando se modifican los márgenes.
 * Recalcula la imagen de fondo solicitada para que en el evento de paint
 * solo sea necesario pintar y no hacer más calculos.
 *
 * @par Operación:
 */
void BotonFondo::ajustarFondo(const QPixmap & qpixOriginal, QPixmap & qpixAjustado)
{
/// <ul>
     /// <li> Si no hay imagen original, la imagen ajustada también es nula.
     if (qpixOriginal.isNull())
     {
          qpixAjustado = qpixOriginal;
     }
     else
     {
          /// <li> Si los márgenes son excesivos y no encajan con la imagen,
          /// se escala la imagen directamente deformándola
          if (this->width() < (int)(m_uintDerecho+m_uintIzquierdo) ||
              this->height() < (int)(m_uintSuperior+m_uintInferior) ||
              qpixOriginal.width() < (int)(m_uintDerecho+m_uintIzquierdo) ||
              qpixOriginal.height() < (int)(m_uintSuperior+m_uintInferior))
          {
               qpixAjustado = qpixOriginal.scaled(this->size());
          }
          /// <li> Si todo se correcto, se puede hacer escalado por partes:
          else
          {
          /// <ul>
               /// <li> Se inicializan las variables.
               qpixAjustado = QPixmap(this->size());
               qpixAjustado.fill(QColor(0, 0, 0, 0));
               QPainter  painter(&qpixAjustado);
               QRect     qrectOrigen, qrectDestino;

               /// <li> Se pinta la esquina superior izquierda.
               qrectOrigen = QRect(0,
                                   0,
                                   m_uintIzquierdo,
                                   m_uintSuperior);
               qrectDestino = qrectOrigen;
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la esquina superior derecha.
               qrectOrigen = QRect(qpixOriginal.width() - m_uintDerecho,
                                   0,
                                   m_uintDerecho,
                                   m_uintSuperior);
               qrectDestino = QRect(this->width() - m_uintDerecho,
                                    0,
                                    m_uintDerecho,
                                    m_uintSuperior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la barra superior
               qrectOrigen = QRect(m_uintIzquierdo,
                                   0,
                                   qpixOriginal.width() - m_uintDerecho - m_uintIzquierdo,
                                   m_uintSuperior);
               qrectDestino = QRect(m_uintIzquierdo,
                                    0,
                                    this->width() - m_uintDerecho - m_uintIzquierdo,
                                    m_uintSuperior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la barra lateral izquierda.
               qrectOrigen = QRect(0,
                                   m_uintSuperior,
                                   m_uintIzquierdo,
                                   qpixOriginal.height() - m_uintSuperior - m_uintInferior);
               qrectDestino = QRect(0,
                                    m_uintSuperior,
                                    m_uintIzquierdo,
                                    this->height() - m_uintSuperior - m_uintInferior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la barra lateral derecha.
               qrectOrigen = QRect(qpixOriginal.width() - m_uintDerecho,
                                   m_uintSuperior,
                                   m_uintDerecho,
                                   qpixOriginal.height() - m_uintSuperior - m_uintInferior);
               qrectDestino = QRect(this->width() - m_uintDerecho,
                                    m_uintSuperior,
                                    m_uintIzquierdo,
                                    this->height() - m_uintSuperior - m_uintInferior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la zona central.
               qrectOrigen = QRect(m_uintIzquierdo,
                                   m_uintSuperior,
                                   qpixOriginal.width() - m_uintDerecho - m_uintIzquierdo,
                                   qpixOriginal.height() - m_uintSuperior - m_uintInferior);
               qrectDestino = QRect(m_uintIzquierdo,
                                    m_uintSuperior,
                                    this->width() - m_uintDerecho - m_uintIzquierdo,
                                    this->height() - m_uintSuperior - m_uintInferior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la esquina inferior izquierda.
               qrectOrigen = QRect(0,
                                   qpixOriginal.height() - m_uintInferior,
                                   m_uintIzquierdo,
                                   m_uintInferior);
               qrectDestino = QRect(0,
                                    this->height() - m_uintInferior,
                                    m_uintIzquierdo,
                                    m_uintInferior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la esquina inferior derecha.
               qrectOrigen = QRect(qpixOriginal.width() - m_uintDerecho,
                                   qpixOriginal.height() - m_uintInferior,
                                   m_uintDerecho,
                                   m_uintInferior);
               qrectDestino = QRect(this->width() - m_uintDerecho,
                                    this->height() - m_uintInferior,
                                    m_uintDerecho,
                                    m_uintInferior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se pinta la barra inferior.
               qrectOrigen = QRect(m_uintIzquierdo,
                                   qpixOriginal.height() - m_uintInferior,
                                   qpixOriginal.width() - m_uintDerecho - m_uintIzquierdo,
                                   m_uintInferior);
               qrectDestino = QRect(m_uintIzquierdo,
                                    this->height() - m_uintInferior,
                                    this->width() - m_uintDerecho - m_uintIzquierdo,
                                    m_uintInferior);
               painter.drawPixmap(qrectDestino, qpixOriginal, qrectOrigen);

               /// <li> Se termina el pintado.
               painter.end();
          /// </ul>
          }
     }
/// </ul>
}

///////////////////////// EVENTOS /////////////////////
/**
 * @brief Evento de cambio de tamaño.
 *
 * @param [in] event          Datos del evento.
 *
 * Evento de cambio de tamaño.
 * Se recalcula el tamaño de los fondos para que sean del
 * tamaño del botón actual y se llama al evento de cambio
 * de tamaño de la clase padre.
 */
void BotonFondo::resizeEvent(QResizeEvent * event)
{
     this->ajustarFondos();
     QPushButton::resizeEvent(event);
}

/**
 * @brief Evento de pintado.
 *
 * @param [in] event          Datos del evento.
 *
 * Evento de pintado.
 * Se pinta la imagen de fondo del botón y para
 * pintar el texto se utilizan las funciones del
 * estilo que esté cargado.
 *
 * @par Operación:
 */
void BotonFondo::paintEvent(QPaintEvent * event)
{
/// <ul>
     Q_UNUSED(event);

     /// <li> Se pinta el fondo dependiendo del estado.
     QPainter  painter(this);
     if (this->isChecked() || this->isDown())
          painter.drawPixmap(0, 0, m_qpixSeleccionado);
     else
          painter.drawPixmap(0, 0, m_qpixFondo);
     painter.end();

     /// <li> Se pinta el resto utilizando las funciones de estilo.
     QStylePainter p(this);
     //StyleOptionButtonMarrodan subopt;
     QStyleOptionButton subopt;
	 subopt.initFrom(this);
     subopt.state = QStyle::State_Enabled;
     subopt.text = this->text();
     subopt.icon = this->icon();
     subopt.iconSize = this->iconSize();
     subopt.features = QStyleOptionButton::Flat;
//     subopt.alignment = m_qalgnAlineacion;

     /// <li> Se ajusta el rectángulo de texto a partir de los márgenes.
     if (m_uintInferior > m_uintSuperior)
          subopt.rect.setBottom(subopt.rect.bottom() - (m_uintInferior-m_uintSuperior));
     else if (m_uintInferior < m_uintSuperior)
          subopt.rect.setTop(subopt.rect.top() + (m_uintSuperior-m_uintInferior));

     /// <li> Se pinta el icono en función de la alineación
     /// y se ajusta el espacio para el texto:
     if (!icon().isNull())
     {
     /// <ul>
          /// <li> Si la alineación es arriba, se pinta el icono arriba y se modifica
          /// el cuadro de texto para que éste aparezca abajo.
          if (m_qalgnAlineacionIcono & Qt::AlignTop)
          {
               QPixmap   qpixIcono;
               if (!this->isEnabled())
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Disabled);
               else
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Normal);
               int iIconW = qpixIcono.width();
               int iIconH = this->iconSize().height();
               int iMargin = 0;
               if (!this->text().isEmpty())
                    iMargin = 4;

               // Se calcula la posicion desde la que pintar.
               QRect qrctBoton = this->style()->subElementRect(QStyle::SE_PushButtonContents, &subopt, this);
               int iXpos = qrctBoton.left() + qrctBoton.width()/2 - iIconW/2;
               int iYpos = qrctBoton.top() + 4;

               // Se pinta el icono.
               p.drawPixmap(iXpos, iYpos, qpixIcono);

               // Se ajusta el cuadro para el texto.
               subopt.rect.setTop(subopt.rect.top() + iIconH + iMargin);
          }

          /// <li> Si la alineación es abajo, se pinta el icono abajo y se modifica
          /// el cuadro de texto para que éste aparezca arriba.
          else if (m_qalgnAlineacionIcono & Qt::AlignBottom)
          {
               QPixmap   qpixIcono;
               if (!this->isEnabled())
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Disabled);
               else
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Normal);

               int iIconW = qpixIcono.width();
               int iIconH = this->iconSize().height();
               int iMargin = 0;
               if (!this->text().isEmpty())
                    iMargin = 4;

               // Se calcula la posicion desde la que pintar.
               QRect qrctBoton = this->style()->subElementRect(QStyle::SE_PushButtonContents, &subopt, this);
               int iXpos = qrctBoton.left() + qrctBoton.width()/2 - iIconW/2;
               int iYpos = qrctBoton.bottom() - 4 - iIconH;
			
               // Se pinta el icono.
               p.drawPixmap(iXpos, iYpos, qpixIcono);

               // Se ajusta el cuadro para el texto.
               subopt.rect.setBottom(subopt.rect.bottom() - iMargin - iIconH);
          }

          /// <li> Si la alineación es derecha, se pinta el icono a la derecha y se modifica
          /// el cuadro de texto para que éste aparezca justo a su izquierda.
          else if (m_qalgnAlineacionIcono & Qt::AlignRight)
          {
               QPixmap   qpixIcono;
               if (!this->isEnabled())
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Disabled);
               else
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Normal);

               int iIconW = qpixIcono.width();
               int iIconH = qpixIcono.height();
               int iMargin = 0;
               if (!this->text().isEmpty())
                    iMargin = 10;

               QFontMetrics qfmMetrics = fontMetrics();
               QSize qszTexto = qfmMetrics.size(Qt::TextShowMnemonic, this->text());

               // Se calcula la posicion desde la que pintar.
               QRect qrctBoton = this->style()->subElementRect(QStyle::SE_PushButtonContents, &subopt, this);
               int iEspacio = (qrctBoton.width() - iIconW - iMargin - qszTexto.width())/2;
               int iXpos = qrctBoton.left() + iMargin + qszTexto.width() + iEspacio;
               int iYpos = qrctBoton.top() + qrctBoton.height()/2 - iIconH/2;

               // Se pinta el icono.
               p.drawPixmap(iXpos, iYpos, qpixIcono);

               // Se ajusta el cuadro para el texto.
               subopt.rect.setRight(subopt.rect.right() - iIconW);
          }

          /// <li> El resto de las posibilidades se tomas como izquierda.
          /// Se pinta el icono a la izquierda y se modifica
          /// el cuadro de texto para que éste aparezca justo a su derecha.
          else
          {
               QPixmap   qpixIcono;
               if (!this->isEnabled())
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Disabled);
               else
                    qpixIcono = this->icon().pixmap(this->iconSize(), QIcon::Normal);

               int iIconW = qpixIcono.width();
               int iIconH = qpixIcono.height();
               int iMargin = 0;
               if (!this->text().isEmpty())
                    iMargin = 10;

               QFontMetrics qfmMetrics = fontMetrics();
               QSize qszTexto = qfmMetrics.size(Qt::TextShowMnemonic, this->text());

               // Se calcula la posicion desde la que pintar.
               QRect qrctBoton = this->style()->subElementRect(QStyle::SE_PushButtonContents, &subopt, this);
               int iEspacio = (qrctBoton.width() - iIconW - iMargin - qszTexto.width())/2;
               int iXpos = qrctBoton.left() + iEspacio;
               int iYpos = qrctBoton.top() + qrctBoton.height()/2 - iIconH/2;

               // Se pinta el icono.
               p.drawPixmap(iXpos, iYpos, qpixIcono);

               // Se ajusta el cuadro para el texto.
               subopt.rect.setLeft(subopt.rect.left() + iIconW);
          }
     /// </ul>
     }

     /// <li> Se ajusta color del texto diferente cuando el botón esta hundido o checkeado.
     if (this->isChecked() || this->isDown())
          subopt.palette.setColor(QPalette::ButtonText, subopt.palette.color(QPalette::Active, QPalette::BrightText));
     else if (!this->isEnabled())
          subopt.palette.setColor(QPalette::ButtonText, subopt.palette.color(QPalette::Disabled, QPalette::ButtonText));
     else
          subopt.palette.setColor(QPalette::ButtonText, subopt.palette.color(QPalette::Active, QPalette::ButtonText));

     p.drawControl(QStyle::CE_PushButton, subopt);
/// </ul>
}

/**
 * @brief Indica el tamaño sugerido para el botón
 *
 * @return          El tamaño sugerido para el botón.
 *
 * Indica el tamaño sugerido para el botón.
 * Calcula el tamaño adecuado para el botón teniendo en cuenta su contenido.
 * Normalmente el botón podrá hacerse más grande pero no más pequeño.
 *
 * @par Operación:
 */
QSize BotonFondo::sizeHint() const
{
/// <ul>
     /// <li> Se crea una variable de estilos con las características del botón.
//     StyleOptionButtonMarrodan opt;
     QStyleOptionButton opt;
	 opt.initFrom(this);
     opt.state = QStyle::State_Enabled;
     opt.text = this->text();
     opt.icon = this->icon();
     opt.iconSize = this->iconSize();
     opt.features = QStyleOptionButton::Flat;
//     opt.alignment = m_qalgnAlineacion;

     /// <li> Se ajusta el rectángulo teniendo en cuenta los márgenes del fondo
     if (m_uintInferior > m_uintSuperior)
          opt.rect.setBottom(opt.rect.bottom() - (m_uintInferior-m_uintSuperior));
     else if (m_uintInferior < m_uintSuperior)
          opt.rect.setTop(opt.rect.top() + (m_uintSuperior-m_uintInferior));

     /// <li> Si tiene icono, se calcula el tamaño necesario para éste.
     int iIconH = 0;
     int iIconW = 0;
     int iTextMargin = 0;
     if (!icon().isNull())
     {
          iIconH = opt.iconSize.height();
          iIconW = opt.iconSize.width();
          if (!this->text().isEmpty())       // Margen para el texto si hay icono Y texto.
               iTextMargin = 4;
     }

     /// <li> Se calcula el espacio necesario para el texto.
     QFontMetrics qfmMetrics = fontMetrics();
     QSize qszTexto = qfmMetrics.size(Qt::TextShowMnemonic, this->text());

     /// <li> Se juntan ambos espacios, el del texto y el del icono
     /// dependiendo de la posición del icono.
     if (m_qalgnAlineacionIcono & Qt::AlignTop)
     {
          qszTexto.rheight() += iIconH + iTextMargin;
          if (iIconW > qszTexto.width())
               qszTexto.setWidth(iIconW);
     }
     else if (m_qalgnAlineacionIcono & Qt::AlignBottom)
     {
          qszTexto.rheight() += iIconH + iTextMargin;
          if (iIconW > qszTexto.width())
               qszTexto.setWidth(iIconW);
     }
     else
     {
          qszTexto.rwidth() += iIconW + iTextMargin;
          if (iIconH > qszTexto.height())
               qszTexto.setHeight(iIconH);
     }


     /// <li> Se calcula el espacio final teniendo en cuenta el margen del botón
     return (style()->sizeFromContents(QStyle::CT_PushButton, &opt, qszTexto, this).expandedTo(QApplication::globalStrut()));
/// </ul>
}
