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

    //ESTILO SPOTIFYYYYY
    setStyleSheet("background-color: #121212; font-family: 'Segoe UI';");

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setAlignment(Qt::AlignTop);

    // Datos del artista
    lblNombre =new QLabel("Bienvenido, " + artista.getNombreArtistico());
    lblNombre->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    lblNombre->setAlignment(Qt::AlignCenter);

    //estilo imagen circular
    lblFoto=new QLabel();
    lblFoto->setFixedSize(130, 130);
    lblFoto->setStyleSheet(
        "border-radius: 65px;"
        "border: 3px solid #1DB954;"
        "background-color: #222;"
        );

    //AQUI SE APLICA UNA SOMBRA REAL CON QGraphicsDropShadowEffect
    QGraphicsDropShadowEffect*sombra=new QGraphicsDropShadowEffect(this);
    sombra->setBlurRadius(15);
    sombra->setOffset(0,0);
    sombra->setColor(QColor("#1DB954"));
    lblFoto->setGraphicsEffect(sombra);

    //========CODIGO NUEVOOOOO===========================                                                  |

    //SE CARGA LA IMAGEN CIRCULAR
    QPixmap pix(artista.getRutaImagen());
    if(!pix.isNull())//VERIFICA SI LA IMAGEN SE CARGO BIEN, SI NO EXISTE PIX NULL DEVULVE TRUE
    {

        //AQUI RECORTO EL CENTRO CUADRADO DE LA IMAGEN ORIGINAL
        int lado=qMin(pix.width(),pix.height());
        QRect centro((pix.width()-lado)/2,(pix.height()-lado)/2,lado,lado);//RECORTO
        QPixmap cuadrado=pix.copy(centro);//ME AYUDA A RECORTAR EL AREA DEFINIDA POR 'CENTRO' DE LA IMAGEN ORIGINAL
        //COPY DEVUELVE UN NUEVO QPIXMAP QUE CONTIENE SOLO LA PARTE DEL RECTANGULO INDICADO.

        //AQUI ESCALO EL TAMANIO DEL LABEL, EJEM 130X130
        //CON SCALED CAMBIO EL TAMANIO DEL QPIXMAP
        //CON Qt::IgnoreAspectRatio: ignora la proporciOn original (porque ya es cuadrado).
        //Qt::SmoothTransformation: usa interpolaciOn bilineal para suavizar bordes y que no se vea pixelado o feo.
        QPixmap escalado=cuadrado.scaled(lblFoto->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

        // Creamos pixmap transparente para recortar el circulo
        //Este sera el lienzo donde dibujaremos la versiOn circular.
        QPixmap CircularPixmap(lblFoto->size());//Crea un nuevo QPixmap vacIo con el mismo tamaño que el lblFoto
        CircularPixmap.fill(Qt::transparent);//Rellena el pixmap con fondo transparente.


        QPainter painter(&CircularPixmap);//Crea un "pincel" (QPainter) para dibujar dentro del CircularPixmap.
        painter.setRenderHint(QPainter::Antialiasing);//Activa el antialiasing, que suaviza los bordes curvos (como los circulos) y evita que se vean pixelados.
        QPainterPath path;//Crea una ruta (forma) que podemos usar para limitar el area de dibujo.
        path.addEllipse(0,0,lblFoto->width(),lblFoto->height());
        painter.setClipPath(path);
        painter.drawPixmap(0,0,escalado);
        painter.end();

        //Se asigna el pixmap recortado a circular
        lblFoto->setPixmap(CircularPixmap);

    }
    //========CODIGO NUEVOOOOO===========================                                                   |

    QHBoxLayout*layoutFoto=new QHBoxLayout();
    layoutFoto->addStretch();
    layoutFoto->addWidget(lblFoto);
    layoutFoto->addStretch();

    // Botones
    btnSubirCancion= new QPushButton("🎵 Subir Cancion");
    btnEditarCancion=new QPushButton("✏ Editar Cancion");
    btnEliminarCancion=new QPushButton("🗑 Eliminar Cancion");
    btnMiMusica=new QPushButton("🎧 Mi Musica");
    btnVerEstadisticas=new QPushButton("📊 Ver Estadisticas");
    btnSalir =new QPushButton("🔙 Cerrar Sesion");

    estiloBoton(btnSubirCancion, "#1DB954");//VERDE
    estiloBoton(btnEditarCancion, "#3E8EED");//AZUL
    estiloBoton(btnEliminarCancion, "#E53935");//ROJO
    estiloBoton(btnMiMusica,"#FFC107");//AMARILLO
    estiloBoton(btnVerEstadisticas, "#9C27B0");//PURPURA
    estiloBoton(btnSalir, "#424242");//GRIS OSCURO

    connect(btnSubirCancion,&QPushButton::clicked,this,&MenuAdmin::abrirVentanaSubirCancion);
    // connect(btnMiMusica, &QPushButton::clicked, this, &MenuAdmin::abrirMiMusica);
    connect(btnSalir,&QPushButton::clicked,this,&MenuAdmin::CerrarSesion);

    //ENSABLAMOS LOS LAYOUT
    layoutPrincipal->addWidget(lblNombre);
    layoutPrincipal->addLayout(layoutFoto);
    layoutPrincipal->addSpacing(10);
    layoutPrincipal->addWidget(btnSubirCancion);
    layoutPrincipal->addWidget(btnEditarCancion);
    layoutPrincipal->addWidget(btnEliminarCancion);
    layoutPrincipal->addWidget(btnVerEstadisticas);
    layoutPrincipal->addWidget(btnMiMusica);
    layoutPrincipal->addSpacing(20);
    layoutPrincipal->addWidget(btnSalir);


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
