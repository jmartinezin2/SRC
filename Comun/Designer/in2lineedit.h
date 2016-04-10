#ifndef IN2LINEEDIT_H
#define IN2LINEEDIT_H

#include <QLineEdit>

class In2LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit In2LineEdit(QWidget *parent = 0);
    
signals:

protected:
     virtual void focusInEvent( QFocusEvent *e );
     virtual void mousePressEvent ( QMouseEvent * e );

public slots:
    
};

#endif // IN2LINEEDIT_H
