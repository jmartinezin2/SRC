#ifndef STATUS_H
#define STATUS_H

#include <QObject>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
#include "teclado.h"
#include "ui_Statusui.h"
#include "in2defines.h"



//Para controlar donde estaba el focus*/
enum FOCUS{
    FOCUS_USER=0,
    FOCUS_PASSWORD
};

class Status : public QWidget
{
	Q_OBJECT

public:
	Status(QWidget *parent);
	~Status();
    //void SetFontSize(int size);
	void EnableWindow();
    void CambioEnPassword();

private:
	void MuestraImagen(QPixmap Logo);
    void CleanQueueMsg();
    void UsuarioIntroducido();
    void TestLDAP();
    void sltExit();
    bool TestSuperUser();
private:
    /*QGroupBox* m_Mensaje;
	QPushButton *m_Aceptar;
        QTextEdit *m_User;
        QLabel *lbl_User;
        QTextEdit *m_Password;
        QLabel *lbl_Password;*/
        Ui::Data *ui;
        char LastFocus;
        Teclado *SCRTeclado;
        int m_loginCounter;

signals:
    //Señal de que oculto ventana de estado
    void StatusOculto();
    void SendDebugInfo(unsigned char,QString);


private slots:
	void Salir();
    void UpdateCampo(QString data);
    void on_m_User_selectionChanged();
    void on_m_Password_selectionChanged(int old, int inew);
    void TestEnter();
    bool eventFilter(QObject *target, QEvent *event);
};

#endif // STATUS_H
