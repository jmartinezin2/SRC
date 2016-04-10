#ifndef IN2FONT_H
#define IN2FONT_H

#include <QObject>
#include <QStringList>

class In2Font : public QObject
{
    Q_OBJECT
public:
    explicit In2Font(QObject *parent = 0);
    ~In2Font();
    QStringList getFontStyleList();
    QList<int> getQtFontStyleList();
    int getQtFontWeight(int index);
    int getQtFontWeight(QString name);
    int getFontStyleIndex(QString font);

private:
    QStringList m_EstiloFuente;
    QList<int> m_EstiloFuenteQt;


signals:

public slots:
};

#endif // IN2FONT_H
