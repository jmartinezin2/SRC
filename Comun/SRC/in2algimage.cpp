#include "in2algimage.h"




In2AlgImage::In2AlgImage(QObject *parent) :
    QObject(parent)
{
}
/** Devuelve el valor maximo de la tabla de color*/
QRgb In2AlgImage::getMaxColorTableValue(QImage & image){
    QRgb rtn=0;
    if (image.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=image.colorTable();
        for (int x=0;x<table.count();x++){
            if (table.at(x)>rtn) rtn=table.at(x);
        }
    }
    else{
        for (int x=0;x<image.width();x++){
            for (int y=0;y<image.height();y++){
                if (image.pixel(x,y)>rtn) rtn=image.pixel(x,y);
            }
        }
    }
    return rtn;
}

/** Devuelve el valor minimo de la tabla de color*/
QRgb In2AlgImage::getMinColorTableValue(QImage & image){
    QRgb rtn=0;
    if (image.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=image.colorTable();
        for (int x=0;x<table.count();x++){
            if (table.at(x)<rtn) rtn=table.at(x);
        }
    }
    else{
        for (int x=0;x<image.width();x++){
            for (int y=0;y<image.height();y++){
                if (image.pixel(x,y)<rtn) rtn=image.pixel(x,y);
            }
        }
    }
    return rtn;
}

/** Decrementa una tabla de color en porcentaje*/
QImage In2AlgImage::aclaraImagen(const QImage & image,int x){
    QImage rtn=image;
    if (image.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=image.colorTable();
        for (int index=0;index<table.count();index++){
            int grey=QColor(table.at(index)).red();
            grey=(grey*(x+100))/100;
            if (grey>255) grey=255;
            table.replace(index,QColor(grey,grey,grey).rgb());
        }
        rtn.setColorTable(table);
    }
    return rtn;
}

/** Incrementa una tabla de color*/
QImage In2AlgImage::oscureceImagen(const QImage & image,int x){
    QImage rtn=image;
    if (image.format()==QImage::Format_Indexed8){
        QVector<QRgb> table=image.colorTable();
        for (int index=0;index<table.count();index++){
            int grey=QColor(table.at(index)).red();
            if (grey<255) grey=(grey*100)/(x+100);
            table.replace(index,QColor(grey,grey,grey).rgb());
        }
        rtn.setColorTable(table);
    }
    return rtn;
}
/** Aplica Floyd-Steimberg*/
QImage In2AlgImage::getFloydSteimberg(QImage & image, QVector<QRgb> &xusbTable){
       //QVector<QRgb> table=image.colorTable();
    QVector<QRgb> table;
    table.resize(256);
       for (int x=0;x<8;x++){
           table.replace(x,xusbTable.at(0));
       }
       for (int x=8;x<54;x++){
           table.replace(x,xusbTable.at(1));
       }
       for (int x=54;x<90;x++){
           table.replace(x,xusbTable.at(2));
       }
       for (int x=90;x<126;x++){
           table.replace(x,xusbTable.at(3));
       }
       for (int x=126;x<162;x++){
           table.replace(x,xusbTable.at(4));
       }
       for (int x=162;x<198;x++){
           table.replace(x,xusbTable.at(5));
       }
       for (int x=198;x<234;x++){
           table.replace(x,xusbTable.at(6));
       }
       for (int x=234;x<255;x++){
           table.replace(x,xusbTable.at(7));
       }

       QImage newImage=image;
       int width=image.width();
       int height=image.height();
       QVector<QRgb> oldTable=image.colorTable();
       QRgb OldColor;
       QRgb NewColor;
       for (int y=0;y<height;y++){
           for (int x=0;x<width;x++){
                OldColor=newImage.pixel(x,y);
                int newColorIndex=returnClosestValue(table,OldColor);
                NewColor=table.at(newColorIndex);
                //newImage.setPixel(x,y,newColorIndex);
                newImage.setPixel(x,y,NewColor);

                signed long error=OldColor-NewColor;

                if (error){
                //Propagamos
                    if (x<width){
                        NewColor=newImage.pixel(x+1,y)+7*error/16;
                        newColorIndex=returnClosestValue(table,NewColor);
                        //newImage.setPixel(x+1,y,newColorIndex);
                        newImage.setPixel(x+1,y,NewColor);
                    }
                    if ((x>0)&&(y<height)){
                        NewColor=newImage.pixel(x-1,y+1)+3*error/16;
                        newColorIndex=returnClosestValue(table,NewColor);
                        //newImage.setPixel(x-1,y+1,newColorIndex);
                        newImage.setPixel(x-1,y+1,NewColor);
                    }
                    if (y<height){
                        NewColor=newImage.pixel(x,y+1)+5*error/16;
                        newColorIndex=returnClosestValue(table,NewColor);
                        //newImage.setPixel(x,y+1,newColorIndex);
                        newImage.setPixel(x,y+1,NewColor);
                    }
                    if ((x<width)&&(y<height)){
                        NewColor=newImage.pixel(x+1,y+1)+1*error/16;
                        newColorIndex=returnClosestValue(table,NewColor);
                        //newImage.setPixel(x+1,y+1,newColorIndex);
                        newImage.setPixel(x+1,y+1,NewColor);
                    }
                }
           }
       }

       //newImage.setColorTable(table);
       return newImage;
}

int In2AlgImage::returnClosestValue(QVector<QRgb> &xusbTable,QRgb value){
    unsigned int newError=255;
    unsigned int oldError=255;
    int index=0;

    newError=qAbs(xusbTable.at(0)-value);
    if (newError<oldError){
        oldError=newError;
        index=0;
    }
    newError=qAbs(xusbTable.at(8)-value);
    if (newError<oldError){
        oldError=newError;
        index=8;
    }
    newError=qAbs(xusbTable.at(54)-value);
    if (newError<oldError){
        oldError=newError;
        index=54;
    }
    newError=qAbs(xusbTable.at(90)-value);
    if (newError<oldError){
        oldError=newError;
        index=90;
    }
    newError=qAbs(xusbTable.at(126)-value);
    if (newError<oldError){
        oldError=newError;
        index=126;
    }
    newError=qAbs(xusbTable.at(162)-value);
    if (newError<oldError){
        oldError=newError;
        index=162;
    }
    newError=qAbs(xusbTable.at(198)-value);
    if (newError<oldError){
        oldError=newError;
        index=198;
    }
    newError=qAbs(xusbTable.at(234)-value);
    if (newError<oldError){
        oldError=newError;
        index=234;
    }
    return index;

}

/** Aplica Floyd-Steimberg*/
QImage In2AlgImage::setNewXusbTable(QImage & image, QVector<QRgb> &xusbTable){
       QVector<QRgb> table=image.colorTable();
       for (int x=0;x<8;x++){
           table.replace(x,xusbTable.at(0));
       }
       for (int x=8;x<54;x++){
           table.replace(x,xusbTable.at(1));
       }
       for (int x=54;x<90;x++){
           table.replace(x,xusbTable.at(2));
       }
       for (int x=90;x<126;x++){
           table.replace(x,xusbTable.at(3));
       }
       for (int x=126;x<162;x++){
           table.replace(x,xusbTable.at(4));
       }
       for (int x=162;x<198;x++){
           table.replace(x,xusbTable.at(5));
       }
       for (int x=198;x<234;x++){
           table.replace(x,xusbTable.at(6));
       }
       for (int x=234;x<255;x++){
           table.replace(x,xusbTable.at(7));
       }

       QImage rtn=image;
       rtn.setColorTable(table);
       return rtn;
}

/** Replica imagen y regenera la tabla*/
QImage In2AlgImage::copyAndGenImage256(QImage & image){
    QVector<QRgb> OriginalTable=image.colorTable();
    QVector<QRgb> table;
    table.resize(256);
    int numColors=OriginalTable.count();
    if (numColors==4){
        for (int x=0;x<8;x++){
            table.replace(x,OriginalTable.at(0));
        }
        for (int x=8;x<54;x++){
            table.replace(x,OriginalTable.at(1));
        }
        for (int x=54;x<90;x++){
            table.replace(x,OriginalTable.at(1));
        }
        for (int x=90;x<126;x++){
            table.replace(x,OriginalTable.at(2));
        }
        for (int x=126;x<162;x++){
            table.replace(x,OriginalTable.at(2));
        }
        for (int x=162;x<198;x++){
            table.replace(x,OriginalTable.at(2));
        }
        for (int x=198;x<234;x++){
            table.replace(x,OriginalTable.at(3));
        }
        for (int x=234;x<=255;x++){
            table.replace(x,OriginalTable.at(3));
        }
    }

    else if ((numColors==8)){
        for (int x=0;x<8;x++){
            table.replace(x,OriginalTable.at(0));
        }
        for (int x=8;x<54;x++){
            table.replace(x,OriginalTable.at(1));
        }
        for (int x=54;x<90;x++){
            table.replace(x,OriginalTable.at(2));
        }
        for (int x=90;x<126;x++){
            table.replace(x,OriginalTable.at(3));
        }
        for (int x=126;x<162;x++){
            table.replace(x,OriginalTable.at(4));
        }
        for (int x=162;x<198;x++){
            table.replace(x,OriginalTable.at(5));
        }
        for (int x=198;x<234;x++){
            table.replace(x,OriginalTable.at(6));
        }
        for (int x=234;x<=255;x++){
            table.replace(x,OriginalTable.at(7));
        }
    }

    QImage rtn=QImage(image.size(),QImage::Format_Indexed8);
    rtn.setColorTable(table);
    for (int x=0;x<image.width();x++){
        for (int y=0;y<image.height();y++){
            QRgb color=image.pixel(x,y);
            int index=OriginalTable.indexOf(color);
            int newIndex=index;
            if (numColors==8){
                if (index==0) newIndex=0;
                else if (index==1) newIndex=25;
                else if (index==2) newIndex=75;
                else if (index==3) newIndex=100;
                else if (index==4) newIndex=150;
                else if (index==5) newIndex=175;
                else if (index==6) newIndex=210;
                else newIndex=255;
            }
            else if (numColors==4){
                if (index==0) newIndex=0;
                else if (index==1) newIndex=75;
                else if (index==2) newIndex=175;
                else if (index==3) newIndex=255;
            }
            else newIndex=index;

            rtn.setPixel(x,y,newIndex);

        }
    }
    //rtn.setColorTable(table);
    return rtn;
}



