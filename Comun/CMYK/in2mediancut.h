#ifndef IN2MEDIANCUT_H
#define IN2MEDIANCUT_H

#include <QVector>
#include <QColor>

static int xusb_4map[]={255,233,161,53,0};

struct strMCPal{
    QVector<QRgb> pal;
    int index;
};

class In2MedianCut
{
public:
    In2MedianCut();
    QVector<QRgb> medianCut(QVector<QRgb> palette,int colors,bool corr);
    struct strMCPal sortPalette(QVector<QRgb> palette);
    QVector<QRgb> sortPaletteIn2(QVector<QRgb> palette);
private:
    bool m_bCoorDinamica;

};

#endif // IN2MEDIANCUT_H
