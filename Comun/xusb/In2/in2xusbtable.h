#ifndef IN2XUSBTABLE_H
#define IN2XUSBTABLE_H

#include <QVector>
#include <QColor>

class In2XusbTable
{
public:
    In2XusbTable();
    QVector<QRgb> tableFromPl(QVector<int> & table);
    QVector<QRgb> tableFromIndex(QVector<int> & table);
    QVector<QRgb> tableFromTable(QVector<QRgb> &table);
};

#endif // IN2XUSBTABLE_H
