#include "menuadmin.h"
#include"subir_cancion.h"
#include<QGraphicsDropShadowEffect>
#include<QPainter>
#include<QPainterPath>
#include"menuinicio.h"

MenuAdmin::MenuAdmin(const Artista& artistaActivo, QWidget *parent):QWidget(parent), artista(artistaActivo)
{

    setWindowTitle("Panel del Artista");
    setFixedSize(700,600);
    configurarUI();

}

void MenuAdmin::configurarUI()
{
    setStyleSheet("background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #191414, stop:1 #121212); font-family: 'Segoe UI';");

    setFixedSize(1000, 700);

    QHBoxLayout *layoutGeneral = new QHBoxLayout(this);

    // ======================== COLUMNA IZQUIERDA =========================
    QWidget *panelLateral = new QWidget;
    panelLateral->setFixedWidth(280);
    panelLateral->setStyleSheet("background-color: #000000; border-top-right-radius: 30px; border-bottom-right-radius: 30px;");

    QVBoxLayout *layoutLateral=new QVBoxLayout(panelLateral);
    layoutLateral->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    layoutLateral->setSpacing(25);

    //Imagen circular
    lblFoto=new QLabel;
    lblFoto->setFixedSize(120, 120);
    lblFoto->setStyleSheet("border-radius: 60px; border: 3px solid #1DB954; background-color: #222;");

    QGraphicsDropShadowEffect *sombra=new QGraphicsDropShadowEffect(this);
    sombra->setBlurRadius(20);
    sombra->setOffset(0,0);
    sombra->setColor(QColor("#1DB954"));
    lblFoto->setGraphicsEffect(sombra);

    QPixmap pix(artista.getRutaImagen());
    if(!pix.isNull())
    {

        int lado=qMin(pix.width(), pix.height());
        QRect centro((pix.width() -lado)/2,(pix.height()-lado)/2,lado,lado);
        QPixmap cuadrado= pix.copy(centro);
        QPixmap escalado= cuadrado.scaled(lblFoto->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QPixmap circular(lblFoto->size());
        circular.fill(Qt::transparent);
        QPainter painter(&circular);
        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addEllipse(0,0,lblFoto->width(),lblFoto->height());
        painter.setClipPath(path);
        painter.drawPixmap(0,0,escalado);
        painter.end();

        lblFoto->setPixmap(circular);

    }

    lblNombre=new QLabel(artista.getNombreArtistico());
    lblNombre->setStyleSheet("font-size: 20px; font-weight: bold; color: #1DB954;");
    lblNombre->setAlignment(Qt::AlignCenter);

    // ================== BOTONES ESTILO MENU ==================

    btnSubirCancion=new QPushButton("➕  Subir Canción");
    btnEditarCancion=new QPushButton("✏️  Editar Canción");
    btnEliminarCancion=new QPushButton("🗑️  Eliminar Canción");
    btnMiMusica=new QPushButton("🎧  Mi Música");
    btnVerEstadisticas=new QPushButton("📊  Ver Estadísticas");
    btnSalir=new QPushButton("🔙  Cerrar Sesión");

    QVector<QPushButton*>botones=
    {

        btnSubirCancion, btnEditarCancion, btnEliminarCancion,
        btnMiMusica, btnVerEstadisticas, btnSalir

    };

    for(QPushButton*boton:botones)
    {

        boton->setFixedHeight(45);
        boton->setCursor(Qt::PointingHandCursor);
        boton->setStyleSheet(
            "QPushButton {"
            "background-color: transparent;"
            "color: white;"
            "text-align: left;"
            "padding-left: 20px;"
            "font-size: 16px;"
            "border: none;"
            "}"
            "QPushButton:hover {"
            "background-color: #1DB954;"
            "color: black;"
            "border-radius: 10px;"
            "}"
            );
        layoutLateral->addWidget(boton);

    }

    layoutLateral->addStretch();
    layoutLateral->insertWidget(0,lblFoto);
    layoutLateral->insertWidget(1,lblNombre);

    // ======================== PANEL DERECHO (estetico vacio por ahora) =========================
    QWidget*panelDerecho=new QWidget;
    panelDerecho->setStyleSheet("background-color: #181818; border-radius: 15px;");
    panelDerecho->setMinimumWidth(680);

    QLabel* lblMensaje=new QLabel("¡Bienvenido al panel de artista!\nSelecciona una opción a la izquierda.");
    lblMensaje->setAlignment(Qt::AlignCenter);
    lblMensaje->setStyleSheet("color: white; font-size: 22px; font-weight: bold;");

    QVBoxLayout* layoutDerecho = new QVBoxLayout(panelDerecho);
    layoutDerecho->addStretch();
    layoutDerecho->addWidget(lblMensaje);
    layoutDerecho->addStretch();

    // Ensamble final
    layoutGeneral->addWidget(panelLateral);
    layoutGeneral->addWidget(panelDerecho);

    // Conexiones
    connect(btnSubirCancion, &QPushButton::clicked, this, &MenuAdmin::abrirVentanaSubirCancion);
    connect(btnSalir, &QPushButton::clicked, this, &MenuAdmin::CerrarSesion);
}

void MenuAdmin::estiloBoton(QPushButton* boton, const QString& color)
{

    boton->setFixedHeight(45);
    boton->setStyleSheet(QString(
                             "QPushButton {"
                             "background-color: %1;"
                             "color: white;"
                             "border: none;"
                             "border-radius: 22px;"
                             "font-size: 16px;"
                             "font-weight: bold;"
                             "}"
                             "QPushButton:hover {"
                             "background-color: white;"
                             "color: %1;"
                             "border: 1px solid %1;"
                             "}"
                             ).arg(color));

}

void MenuAdmin::abrirVentanaSubirCancion()
{

    Subir_Cancion*v=new Subir_Cancion(artista);
    v->setAttribute(Qt::WA_DeleteOnClose); //Se destruye al cerrar
    v->show();
    this->close();

}
void MenuAdmin::CerrarSesion()
{

    MenuInicio*m=new MenuInicio(nullptr);
    m->show();
    this->close();

}
