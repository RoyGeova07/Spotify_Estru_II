#ifndef PERFILUSUARIO_H
#define PERFILUSUARIO_H

#include<QWidget>
#include<QLabel>
#include<QTextEdit>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QGridLayout>
#include<QPainter>
#include<QPainterPath>
#include"usuario.h"

class PerfilUsuario:public QWidget
{

    Q_OBJECT

public:
    explicit PerfilUsuario(const Usuario&u,QWidget*parent=nullptr);


private:
    Usuario usuario;
    QLabel*foto;
    QLabel*lblNombreUsuario;
    QLabel*lblNombreReal;
    QLabel*lblCorreo;
    QLabel*lblGeneroFav;
    QLabel*lblFechaReg;
    QLabel*lblFechaNac;
    QLabel*lblRol;
    QLabel*lblEstado;
    QLabel*lblId;

    void cargarFotoCircular(const QString& ruta, const QSize& size);

    void VolverHome();

};

#endif // PERFILUSUARIO_H
