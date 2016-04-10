#ifndef IN2CONFIGURATORDIAMETER_H
#define IN2CONFIGURATORDIAMETER_H


class In2ConfiguratorDiameter
{
public:
    In2ConfiguratorDiameter(int sensor);
    bool getEnabled();
    bool getPLCEnabled();

private:
    bool monitorEnabled;
    bool PLCEnabled;
    int iPuerto;
    double cal_db1;
    double cal_s1;
    double cal_db2;
    double cal_s2;


};

#endif // IN2CONFIGURATORDIAMETER_H
