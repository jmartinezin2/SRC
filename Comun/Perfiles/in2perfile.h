#ifndef IN2PERFILE_H
#define IN2PERFILE_H

#include <QVector>

class In2Perfile
{
public:
    In2Perfile();
    ~In2Perfile();

    QVector<int> getPalette();
    int getBrillo();
    float getContraste();
    void setBrillo(int x);
    void setContraste(float x);
    void setPalette(QVector<int> & pal);
    void setCorreccionBlancos(bool val);
    bool getCorreccionBlancos();
    void setDifAlgorithm(int val);
    int  getDifAlgorithm();
    void setMonoMode(bool val);
    bool getMonoMode();



private:
    QVector<int> m_palette;
    int m_brillo;
    float m_contraste;
    bool m_CorrBlancos;
    bool m_monoMode;
    int m_difAlgo;


};

#endif // IN2PERFILE_H
