#ifndef IN2EDITORPERMISOS_H
#define IN2EDITORPERMISOS_H

#include <QWidget>
#include "ui_in2permisosui.h"
#include <QVector>
#include <QLabel>
#include <QCheckBox>

class In2EditorPermisos : public QWidget
{
    Q_OBJECT
public:
    explicit In2EditorPermisos(QWidget *parent = 0);

private slots:
    void sltExit();
    void sltSave();
    void sltLoadTable(QString table);
signals:
    void SendDebugInfo(unsigned char value,QString txt);
private:
    QVector<QLabel*> m_item;
    QVector<int> m_permiso;
    QVector<QCheckBox *> m_printer;
    QVector<QCheckBox *> m_admin;
    QVector<QCheckBox *> m_Maintenance;
    QVector<QCheckBox *> m_supersuse;
    QStringList m_permisos;

    Ui::In2PermisosUI *ui;
public slots:
};

#endif // IN2EDITORPERMISOS_H
