#include "perfilusuario.h"
#include <QPixmap>
#include"home.h"


PerfilUsuario::PerfilUsuario(const Usuario&u,QWidget*parent):QWidget(parent),usuario(u)
{

    setWindowTitle("Perfil de Usuario");
    setFixedSize(700, 520);
    setStyleSheet("background-color:#121212; color:white;");

    auto root=new QVBoxLayout(this);
    root->setContentsMargins(24,24,24,24);
    root->setSpacing(18);

    auto header=new QLabel("Mi Perfil");
    header->setStyleSheet("font-size:22px; font-weight:700;");
    root->addWidget(header);

    //Superior: foto + nombre/grilla
    auto top=new QHBoxLayout;
    top->setSpacing(20);

    foto=new QLabel;
    foto->setFixedSize(140,140);
    foto->setStyleSheet("border-radius:70px; border:3px solid #1DB954; background:#222;");
    top->addWidget(foto,0,Qt::AlignTop);
    cargarFotoCircular(usuario.getRutaFoto(),foto->size());

    auto right=new QVBoxLayout;

    lblNombreUsuario=new QLabel(usuario.getNombreUsuario());
    lblNombreUsuario->setStyleSheet("font-size:26px; font-weight:800;");
    right->addWidget(lblNombreUsuario);


    auto grid=new QGridLayout; grid->setHorizontalSpacing(16);grid->setVerticalSpacing(8);

    auto mk=[](const QString& k, const QString& v)
    {

        QWidget*w=new QWidget;auto h=new QHBoxLayout(w);h->setContentsMargins(0,0,0,0);
        auto lk=new QLabel(k + ":");lk->setStyleSheet("color:#b3b3b3; font-size:14px;");
        auto lv=new QLabel(v);lv->setStyleSheet("color:white;   font-size:14px;");
        h->addWidget(lk); h->addWidget(lv,1);return w;

    };

    lblId=new QLabel(QString::number(usuario.getId()));
    lblNombreReal=new QLabel(usuario.getNombreReal());
    lblCorreo=new QLabel(usuario.getCorreo());
    lblGeneroFav=new QLabel(usuario.getGeneroFavorito());
    lblFechaReg=new QLabel(usuario.getFechaRegistro().toString("yyyy-MM-dd"));
    lblFechaNac=new QLabel(usuario.getFechaNacimiento().toString("yyyy-MM-dd"));
    lblRol=new QLabel(usuario.getEsAdmin()?"Administrador":"Usuario");
    lblEstado=new QLabel(usuario.estaActivo()?"Activo":"Inactivo");

    grid->addWidget(mk("ID",lblId->text()),0,0);
    grid->addWidget(mk("Correo",lblCorreo->text()),0,1);
    grid->addWidget(mk("Nombre real",lblNombreReal->text()),1,0);
    grid->addWidget(mk("Género favorito",lblGeneroFav->text()),1,1);
    grid->addWidget(mk("Registro",lblFechaReg->text()),2,0);
    grid->addWidget(mk("Nacimiento",lblFechaNac->text()),2,1);
    grid->addWidget(mk("Rol",lblRol->text()),3,0);
    grid->addWidget(mk("Estado",lblEstado->text()),3,1);

    right->addLayout(grid);
    top->addLayout(right,1);

    root->addLayout(top);

    //Cerrar
    auto btnCerrar=new QPushButton("Cerrar");
    btnCerrar->setStyleSheet("QPushButton{background:#282828;color:white;border:1px solid #444;border-radius:8px;padding:6px 12px;} QPushButton:hover{background:#333;}");
    root->addStretch();
    root->addWidget(btnCerrar,0,Qt::AlignRight);
    connect(btnCerrar,&QPushButton::clicked,this,&PerfilUsuario::VolverHome);

}

void PerfilUsuario::cargarFotoCircular(const QString &ruta, const QSize &size)
{


    QPixmap px(ruta);
    if(px.isNull()){foto->setPixmap(QPixmap());return;}

    int lado=qMin(px.width(),px.height());
    QRect rec((px.width()-lado)/2,(px.height()-lado)/2,lado,lado);
    QPixmap cuadrado=px.copy(rec).scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    QPixmap circular(size);
    circular.fill(Qt::transparent);
    QPainter p(&circular);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path; path.addEllipse(0,0,size.width(),size.height());
    p.setClipPath(path);
    p.drawPixmap(0,0,cuadrado);
    p.end();

    foto->setPixmap(circular);

}

void PerfilUsuario::VolverHome()
{

    Home*h=new Home(usuario,nullptr);
    h->show();
    this->hide();

}
