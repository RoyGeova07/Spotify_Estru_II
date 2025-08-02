#include "menuinicio.h"
#include "registroartista.h"
#include"registrousuario.h"
#include <QApplication>
#include"login.h"

MenuInicio::MenuInicio(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Spotify");
    setFixedSize(600, 450);

    QPalette paleta;
    paleta.setBrush(QPalette::Window, QBrush(QPixmap(":/imagenes/fondo.jpg").scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    setAutoFillBackground(true);
    setPalette(paleta);

    // Título
    lblTitulo =new QLabel("Spotify", this);
    lblTitulo->setAlignment(Qt::AlignCenter);
    lblTitulo->setStyleSheet("color: #1DB954; font-size: 36px; font-weight: bold;");

    // Botones
    btnRegistrarUsuario= new QPushButton("Registrar Usuario");
    btnRegistrarArtista= new QPushButton("Registrar Artista");
    btnIniciarSesion =new QPushButton("Iniciar Sesión");
    btnSalir =new QPushButton("Salir");

    QString estiloBoton=
        "QPushButton {"
        " background-color: #1DB954;"
        " color: white;"
        " border-radius: 10px;"
        " padding: 10px;"
        " font-size: 16px;"
        "}"
        "QPushButton:hover {"
        " background-color: #1ed760;"
        "}";

    btnRegistrarUsuario->setStyleSheet(estiloBoton);
    btnRegistrarArtista->setStyleSheet(estiloBoton);
    btnIniciarSesion->setStyleSheet(estiloBoton);
    btnSalir->setStyleSheet(estiloBoton);

    // Layout
    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(lblTitulo);
    layout->addSpacing(30);
    layout->addWidget(btnRegistrarUsuario);
    layout->addWidget(btnRegistrarArtista);
    layout->addWidget(btnIniciarSesion);
    layout->addWidget(btnSalir);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    setLayout(layout);

    // Conexiones
    connect(btnRegistrarUsuario, &QPushButton::clicked, this, &MenuInicio::abrirRegistroUsuario);
    connect(btnRegistrarArtista, &QPushButton::clicked, this, &MenuInicio::abrirRegistroArtista);
    connect(btnIniciarSesion, &QPushButton::clicked, this, &MenuInicio::abrirLogin);
    connect(btnSalir, &QPushButton::clicked, this, &MenuInicio::salir);
}

void MenuInicio::abrirRegistroUsuario()
{

    this->close();
    RegistroUsuario*r=new RegistroUsuario(this);
    r->show();

}

void MenuInicio::abrirRegistroArtista()
{

    this->close();
    RegistroArtista*r=new RegistroArtista(nullptr);
    r->show();

}

void MenuInicio::abrirLogin()
{

    this->close();
    LogIn*log=new LogIn(this);
    log->show();

}

void MenuInicio::salir()
{

    QApplication::quit();

}
