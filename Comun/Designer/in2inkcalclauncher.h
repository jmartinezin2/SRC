#ifndef IN2INKCALCLAUNCHER_H
#define IN2INKCALCLAUNCHER_H

#include <QObject>
#include <QFile>

class In2InkCalcLauncher : public QObject
{
    Q_OBJECT
public:
    explicit In2InkCalcLauncher(QObject *parent = 0);
    ~In2InkCalcLauncher();
    bool exists();
    void start(QString file);

signals:

private:
    QString m_strExecutable;

public slots:
};

#endif // IN2INKCALCLAUNCHER_H
