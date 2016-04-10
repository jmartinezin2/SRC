#include "in2xusbtable.h"

//Tabla de datos para el xub XUSB
static int xusbData[]={255,210,175,150,105,75,30,0};
//static int xusbData[]={0,30,75,105,150,175,210,255};


In2XusbTable::In2XusbTable()
{

}
/** Devuelve una nueva tabla de color a partir de los indices 7-0*/
QVector<QRgb> In2XusbTable::tableFromIndex(QVector<int> &table){
    QVector<QRgb> rtn;
    if (table.count()==8){
        rtn.resize(256);
        int index=table.at(0);
        int data=xusbData[index];
        QRgb color=qRgb(data,data,data);
        for (int x=0;x<8;x++){
            rtn[x]=color;
        }
        index=table.at(1);
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=8;x<54;x++){
            rtn[x]=color;
        }
        index=table.at(2);
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=54;x<90;x++){
            rtn[x]=color;
        }
        index=table.at(3);
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=90;x<126;x++){
            rtn[x]=color;
        }
        index=table.at(4);
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=126;x<162;x++){
            rtn[x]=color;
        }
        index=table.at(5);
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=162;x<198;x++){
            rtn[x]=color;
        }
        index=table.at(6);
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=198;x<234;x++){
            rtn[x]=color;
        }
        index=table.at(7);
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=234;x<=255;x++){
            rtn[x]=color;
        }
    }
    return rtn;
}
/** Devuelve una nueva tabla de color a partir de los valores de pl 42-0*/
QVector<QRgb> In2XusbTable::tableFromPl(QVector<int> &table){
    QVector<QRgb> rtn;
    if (table.count()==8){
        rtn.resize(256);
        int index=table.at(0)/6;
        int data=xusbData[index];
        QRgb color=qRgb(data,data,data);
        for (int x=0;x<8;x++){
            rtn[x]=color;
        }
        index=table.at(1)/6;
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=8;x<54;x++){
            rtn[x]=color;
        }
        index=table.at(2)/6;
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=54;x<90;x++){
            rtn[x]=color;
        }
        index=table.at(3)/6;
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=90;x<126;x++){
            rtn[x]=color;
        }
        index=table.at(4)/6;
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=126;x<162;x++){
            rtn[x]=color;
        }
        index=table.at(5)/6;
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=162;x<198;x++){
            rtn[x]=color;
        }
        index=table.at(6)/6;
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=198;x<234;x++){
            rtn[x]=color;
        }
        index=table.at(7)/6;
        data=xusbData[index];
        color=qRgb(data,data,data);
        for (int x=234;x<=255;x++){
            rtn[x]=color;
        }

    }
    return rtn;
}

/** Devuelve una nueva tabla de color a partir de uan tabla*/
QVector<QRgb> In2XusbTable::tableFromTable(QVector<QRgb> &table){
    QVector<QRgb> rtn;
    if (table.count()==8){
        rtn.resize(256);
        QRgb color=table.at(0);
        for (int x=0;x<8;x++){
            rtn[x]=color;
        }
        color=table.at(1);
        for (int x=8;x<54;x++){
            rtn[x]=color;
        }
        color=table.at(2);
        for (int x=54;x<90;x++){
            rtn[x]=color;
        }
        color=table.at(3);
        for (int x=90;x<126;x++){
            rtn[x]=color;
        }
        color=table.at(4);
        for (int x=126;x<162;x++){
            rtn[x]=color;
        }
        color=table.at(5);
        for (int x=162;x<198;x++){
            rtn[x]=color;
        }
        color=table.at(6);
        for (int x=198;x<234;x++){
            rtn[x]=color;
        }
        color=table.at(7);
        for (int x=234;x<=255;x++){
            rtn[x]=color;
        }

    }
    return rtn;
}

