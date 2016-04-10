#ifndef IN2INDUKITCONFIGURATOR_H
#define IN2INDUKITCONFIGURATOR_H


class In2IndukitConfigurator
{
public:
    In2IndukitConfigurator();
    int offset();
    int totalWidth();
    int cutWidth();
    void setOffset(int value);
    void setTotalWidth(int value);
    void setCutWidth(int value);
    bool saveConfig();
    void loadConfig();


private:
    int m_offset;
    int m_totalWidth;
    int m_cutWidth;


};

#endif // IN2INDUKITCONFIGURATOR_H
