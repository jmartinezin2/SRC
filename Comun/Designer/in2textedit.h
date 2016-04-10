#ifndef IN2TEXTEDIT_H
#define IN2TEXTEDIT_H

#include <QTextEdit>

class In2TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit In2TextEdit(QWidget *parent = 0);
    
signals:

protected:
    virtual void focusInEvent( QFocusEvent *e );
public slots:
    
};

#endif // IN2TEXTEDIT_H
