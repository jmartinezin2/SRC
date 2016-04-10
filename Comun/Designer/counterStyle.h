#ifndef COUNTERSTYLE_H
#define COUNTERSTYLE_H

#include <QObject>
#include <QLocale>

class counterStyle : public QObject
{
    Q_OBJECT
public:
    explicit counterStyle(QObject *parent = 0);
	~counterStyle();
    QString darformato(quint64 value, struct FormatoNumero & f);
signals:

private:
    QLocale localePrevio;
    QLocale miLocale;

public slots:
    
};

#endif // COUNTERSTYLE_H
