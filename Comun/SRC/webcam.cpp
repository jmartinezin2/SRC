/** @addtogroup Webcam
 *  @{*/
/** @file NivelTintas.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Webcam.cpp
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */


#include "webcam.h"
#include "globals.h"
#include "inputdata.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "principal.h"

#include "inkjetform.h"
#include "maquina.h"

#include "dbgmem.h"

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"


Webcam::Webcam(QWidget *parent)
    : /*InkjetForm(parent)*/ ui(new Ui::WebCamUI)
{
	
    ui->setupUi(this);
    GlobalSetScreenSize(this);


    //ui->lbl_Imagen->setMaximumWidth(ui->lbl_Imagen->width());
    //ui->lbl_Imagen->setMaximumHeight(ui->lbl_Imagen->height());

    connect(ui->m_CloseWindow,SIGNAL(clicked()),this,SLOT(CerrarVentana()));

    m_Temporizador = new QTimer(this);
	m_Temporizador->start(15);
	QObject::connect(m_Temporizador, SIGNAL(timeout()),this, SLOT(UpdateTemporizador()));

	capture = cvCaptureFromCAM( CV_CAP_ANY );
	if (!capture){
		existe_camara=false;
        QMessageBox::warning(0,"ATENCIÓN","No hay camara instalada",QMessageBox::Ok);
	}
	else{
		existe_camara=true;
		IplImage* frame = cvQueryFrame( capture );

        //12102012 Jorge
        /* create window */
        //Para que aparezca la imagen en una ventana distinta,creo que se usa
        //cvNamedWindow( const char* name, int flags CV_DEFAULT(CV_WINDOW_AUTOSIZE) );
        /* display image within window (highgui windows remember their content) */
        //cvShowImage( const char* name, const CvArr* image );

		width = frame->width;
        height= frame->height;
		widthStep = frame->widthStep;
		channels = frame->nChannels;
		bytesperline = width * channels;
			
        qImageBuffer=NULL;

		qImageBuffer = (unsigned char *) malloc (width*height*4*sizeof(unsigned char));

	}

	
}

Webcam::~Webcam()
{
}

void Webcam::CerrarVentana(){

    m_Temporizador->stop();

    cvReleaseCapture(&capture);

    ui->lbl_Imagen->setPixmap(QPixmap());

    free(qImageBuffer);

    this->deleteLater();

}


void Webcam::UpdateTemporizador(){

    //existe_camara=false;

	if (existe_camara==true){
		IplImage* frame = cvQueryFrame( capture );

		if (!frame){
			QMessageBox::warning(0,"ATENCIÓN","Captura incorrecta",QMessageBox::Ok);
		}
		else{

			unsigned char *QImagePtr=qImageBuffer;
			const unsigned char *iplImagePtr = (const unsigned char *) frame->imageData;

			for(int y=0;y<height;y++){
				for(int x=0;x<width;x++){
					QImagePtr[0]=iplImagePtr[0];
					QImagePtr[1]=iplImagePtr[1];
					QImagePtr[2]=iplImagePtr[2];
					QImagePtr[3]=0;

					QImagePtr+=4;
					iplImagePtr+=3;
				}
				iplImagePtr+=widthStep-3*width;
			}

			QImage Imagen_webcam(qImageBuffer,width, height,QImage::Format_RGB32);

            //Antes estaba esta ui->lbl_Imagen->setMinimumSize(width,height);

            //ui->lbl_imagen_webcam->setPixmap(QPixmap::fromImage(Imagen_webcam).scaledToHeight(200));

            ui->lbl_Imagen->setPixmap(QPixmap::fromImage(Imagen_webcam));


            ui->lbl_Imagen->show();

		}
	}
}

