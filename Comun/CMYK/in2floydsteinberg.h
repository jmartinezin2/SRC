#ifndef IN2FLOYDSTEINBERG_H
#define IN2FLOYDSTEINBERG_H

#include <QImage>
#include <QColor>

struct Elem {
  //uchar* scanLine;
  QVector<uchar*> scanLine;

  int lineIndex;
  int lineSize;
  QVector<QRgb> v;
};


class In2FloydSteinberg
{
public:
    In2FloydSteinberg();
    ~In2FloydSteinberg();
    QImage convert(QImage & img,QVector<QRgb> &table);
    QVector<QRgb> getOptimumTable(QImage &img,int colors,bool corr);
    QVector<QRgb> getOptimumTableExt(QImage &img,int colors);
    QVector<QRgb> getOptimumTableMediaCut(QImage &img,int colors,bool corr);
    QVector<QRgb> getOptimumTableIn2(QImage &img,int colors,bool corr);

private:
    //void getPal(Elem &e);
};

#endif // IN2FLOYDSTEINBERG_H
