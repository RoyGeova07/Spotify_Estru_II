#include "login.h"
#include "gestorartistas.h"
#include "gestorusuarios.h"
#include "home.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include<QLabel>
#include"menuadmin.h"
#include"menuinicio.h"

LogIn::LogIn(QWidget *parent) : QDialog(parent)
{

    setWindowTitle("Iniciar Sesión");
    setFixedSize(400,250);

    txtUsuario =new QLineEdit;
    txtContrasena= new QLineEdit;
    txtContrasena->setEchoMode(QLineEdit::Password);

    btnIniciar =new QPushButton("Iniciar Sesión");
    btnVolver =new QPushButton("← Volver");

    QLabel*lblTitulo =new QLabel("Inicio de Sesión");
    lblTitulo->setStyleSheet("font-size: 18px; font-weight: bold;");
    lblTitulo->setAlignment(Qt::AlignCenter);

    QVBoxLayout*layout= new QVBoxLayout(this);
    layout->addWidget(lblTitulo);
    layout->addSpacing(10);
    layout->addWidget(new QLabel("Nombre de Usuario o Nombre real de Artista:"));
    layout->addWidget(txtUsuario);
    layout->addWidget(new QLabel("Contraseña:"));
    layout->addWidget(txtContrasena);
    layout->addSpacing(10);
    layout->addWidget(btnIniciar);
    layout->addWidget(btnVolver);

    connect(btnIniciar,&QPushButton::clicked,this,&LogIn::validarCredenciales);
    connect(btnVolver,&QPushButton::clicked,this,&LogIn::volverr);
}

void LogIn::validarCredenciales()
{

    QString nombre= txtUsuario->text();
    QString contrasena =txtContrasena->text();

    if(nombre.isEmpty()||contrasena.isEmpty())
    {

        QMessageBox::warning(this,"Campos vacíos","Por favor completa todos los campos.");
        return;

    }

    GestorArtistas gestorArtistas;
    Artista artista;
    if(gestorArtistas.validarLogIn(nombre, contrasena, artista))
    {

        QMessageBox::information(this, "Bienvenido", QString("🎵 ¡Bienvenido artista '%1'!").arg(artista.getNombreArtistico()));
        MenuAdmin*h=new MenuAdmin(artista,nullptr);
        h->show();
        this->close();
        return;

    }

    GestorUsuarios gestorUsuarios;
    Usuario usuario;
    if(gestorUsuarios.validarLogin(nombre, contrasena, usuario))
    {

        QMessageBox::information(this, "Bienvenido", QString("¡Bienvenido usuario '%1'!").arg(usuario.getNombreUsuario()));
        Home*h=new Home(usuario,nullptr);
        h->show();
        this->close();
        return;

    }

    QMessageBox::critical(this, "Error", "Credenciales incorrectas o usuario inactivo.");

}

void LogIn::volverr()
{

    MenuInicio*m=new MenuInicio(nullptr);
    m->show();
    this->close();

}
