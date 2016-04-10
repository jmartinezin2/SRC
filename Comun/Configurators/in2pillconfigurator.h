#ifndef IN2PILLCONFIGURATOR_H
#define IN2PILLCONFIGURATOR_H

#include <QObject>
#include <QRect>
#include <QVector>

/** Objeto de pill*/
struct strPill{
    float m_Offset;
    QString m_file;
    int m_VProgram;
    QString m_name;
};

class In2PillConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit In2PillConfigurator(QObject *parent = 0);
    void readConfig();
    QString getPillDiameter();
    QString getX0();
    QString getY0();
    QString getWidth();
    QString getHeight();
    QRect getPrintableRect();
    QVector<struct strPill> getPillVector();

    void setPillDiameter(QString val);
    void setX0(QString val);
    void setY0(QString val);
    void setWidth(QString val);
    void setHeight(QString val);

    void saveConfig();



private:
    QString m_pillDiameter;
    QString m_x0;
    QString m_y0;
    QString m_width;
    QString m_height;
    int m_items;
    QVector<struct strPill> m_pill;

signals:
    
public slots:
    
};

#endif // IN2PILLCONFIGURATOR_H
