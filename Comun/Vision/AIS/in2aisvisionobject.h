#ifndef IN2AISVISIONOBJECT_H
#define IN2AISVISIONOBJECT_H

#include <QObject>

#define AIS_STROCR "0"
#define AIS_STRIMG1 "b1" //Dejamos los dos iguales
#define AIS_STRIMG2 "b1"

enum AIS_VISION_TYPE{
    AIS_OCR=0,
    AIS_IMG1,
    AIS_IMG2
};

class In2AISVisionObject
{

public:
    In2AISVisionObject();
    void setData(int type=0,QString x="", QString y="", QString h="", QString w="", QString str="",QString name="");
    int getType();
    QString getX();
    QString getY();
    QString getH();
    QString getW();
    QString getAngle();
    QString getString();
    QString getName();

    void setType(int val);
    void setX(QString val);
    void setY(QString val);
    void setH(QString val);
    void setW(QString val);
    void setAngle(QString angle);
    void setString(QString str);
    void setName(QString str);

private:
    int m_type;
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    float m_angle;
    QString m_string;
    QString m_name;
    QString m_strX;
    QString m_strY;
    QString m_strWidth;
    QString m_strHeight;
    QString m_strAngle;

    
signals:
    
public slots:
    
};

#endif // IN2AISVISIONOBJECT_H
