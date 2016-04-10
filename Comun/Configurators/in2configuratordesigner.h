#ifndef IN2CONFIGURATORDESIGNER_H
#define IN2CONFIGURATORDESIGNER_H


class In2ConfiguratorDesigner
{
public:
    In2ConfiguratorDesigner();
    ~In2ConfiguratorDesigner();
    bool saveCampos();
    void setSaveCampos(bool val);
    void saveConfig();
    unsigned int getColor1();
    unsigned int getColor2();
    unsigned int getColor3();
    unsigned int getColor4();
    int getNumColors();

private:
    bool m_bSaveCampos;
    unsigned int m_color1;
    unsigned int m_color2;
    unsigned int m_color3;
    unsigned int m_color4;
    int m_numColors;


};

#endif // IN2CONFIGURATORDESIGNER_H
