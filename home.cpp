#include"home.h"
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QLineEdit>
#include<QPushButton>
#include<QScrollArea>
#include<QPixmap>
#include<QMenu>
#include<QSpacerItem>
#include<menuinicio.h>
#include<QPainter>
#include<QPainterPath>
#include<QGraphicsDropShadowEffect>
#include<gestorartistas.h>
#include<QDebug>
#include<QStackedLayout>

Home::Home(const Usuario& usuarioActivo, QWidget *parent): QWidget(parent), usuario(usuarioActivo)
{
    setWindowTitle("Inicio");
    setFixedSize(1100, 700);

    QHBoxLayout*layoutColumnas=new QHBoxLayout(this);  //Columnaa principal

    // ================== COLUMNA IZQUIERDA ==================
    QVBoxLayout *colBiblioteca=new QVBoxLayout();
    colBiblioteca->setAlignment(Qt::AlignTop);
    colBiblioteca->setSpacing(15);

    //"Tu biblioteca" con botón "+" a la derecha
    QHBoxLayout*layoutBibliotecaHeader=new QHBoxLayout();

    QLabel*lblTuBiblioteca=new QLabel("Tu biblioteca");
    lblTuBiblioteca->setStyleSheet("font-weight: bold; font-size: 18px;");
    layoutBibliotecaHeader->addWidget(lblTuBiblioteca);

    layoutBibliotecaHeader->addStretch(); //Con esto empuja el boton a la derecha

    QPushButton*btnAgregar=new QPushButton("➕");
    btnAgregar->setFixedSize(30,30);
    btnAgregar->setCursor(Qt::PointingHandCursor);
    btnAgregar->setStyleSheet(R"(
        QPushButton {
            background-color: #1DB954;
            color: white;
            border: none;
            border-radius: 15px;
            font-size: 18px;
        }
        QPushButton:hover {
            background-color: #1ed760;
        }
    )");
    layoutBibliotecaHeader->addWidget(btnAgregar);

    colBiblioteca->addLayout(layoutBibliotecaHeader);

    //Aqui placeholder de playlists
    QLabel*lblVacio=new QLabel("No hay playlists aún");
    lblVacio->setStyleSheet("font-style: italic; font-size: 14px; color: gray;");
    colBiblioteca->addWidget(lblVacio);

    //Panel lateral izquierdo
    QWidget*panelIzquierdo=new QWidget();
    panelIzquierdo->setLayout(colBiblioteca);
    panelIzquierdo->setFixedWidth(230);
    panelIzquierdo->setStyleSheet("background-color: #121212; color: white; padding: 10px;");

    layoutColumnas->addWidget(panelIzquierdo);

    // ================== COLUMNA DERECHA ==================
    QVBoxLayout*colContenido=new QVBoxLayout();

    // ---------- BARRA SUPERIOR ----------
    QHBoxLayout*barraSuperior=new QHBoxLayout();

    QLabel*lblHome=new QLabel("🏠");
    lblHome->setStyleSheet("font-size: 22px;");
    barraSuperior->addWidget(lblHome);

    QLineEdit *busqueda=new QLineEdit();
    busqueda->setPlaceholderText("¿Qué quieres reproducir?");
    busqueda->setFixedHeight(30);
    busqueda->setStyleSheet("border-radius: 15px; padding-left: 10px;");
    barraSuperior->addWidget(busqueda,1);

    barraSuperior->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    QLabel*lblImagen=new QLabel();
    lblImagen->setFixedSize(40,40);
    lblImagen->setStyleSheet(

        "border-radius: 20px;"
        "border: 2px solid white;"
        "background-color: #222;"

        );
    lblImagen->setCursor(Qt::PointingHandCursor);

    //Aqui imagen circular personalizada
    QPixmap pix(usuario.getRutaFoto());
    if(!pix.isNull())
    {

        int lado=qMin(pix.width(),pix.height());
        QRect centro((pix.width()-lado)/2,(pix.height()-lado)/2,lado,lado);
        QPixmap cuadrado =pix.copy(centro);
        QPixmap escalado= cuadrado.scaled(lblImagen->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QPixmap circular(lblImagen->size());
        circular.fill(Qt::transparent);

        QPainter painter(&circular);
        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addEllipse(0, 0, lblImagen->width(), lblImagen->height());
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, escalado);
        painter.end();

        lblImagen->setPixmap(circular);

        //EFECTO SOMBRAAAAAAA VERDECITOUUUUU
        QGraphicsDropShadowEffect*sombra=new QGraphicsDropShadowEffect(this);
        sombra->setBlurRadius(15);
        sombra->setOffset(0,0);
        sombra->setColor(QColor("#1DB954"));
        lblImagen->setGraphicsEffect(sombra);

    }

    QPushButton *btnVolver = new QPushButton("Volver");
    btnVolver->setCursor(Qt::PointingHandCursor);
    btnVolver->setStyleSheet(R"(
        QPushButton {
            background-color: #282828;
            color: white;
            border: 1px solid white;
            border-radius: 10px;
            padding: 5px 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3e3e3e;
        }
    )");
    connect(btnVolver, &QPushButton::clicked, this, &Home::Regresar);
    barraSuperior->addSpacing(20);
    barraSuperior->addWidget(btnVolver);
    barraSuperior->addSpacing(20);

    QMenu *menuPerfil = new QMenu();
    menuPerfil->addAction("Perfil");
    lblImagen->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(lblImagen, &QLabel::customContextMenuRequested, [=](const QPoint &pos) {
        menuPerfil->exec(lblImagen->mapToGlobal(pos));
    });

    barraSuperior->addWidget(lblImagen);
    colContenido->addLayout(barraSuperior);

    // ---------- AREA DE SCROLL ----------
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    QWidget *contenidoScroll = new QWidget();
    QVBoxLayout *layoutScroll = new QVBoxLayout(contenidoScroll);

    QLabel *lblCreadoPara = new QLabel("Creado para " + usuario.getNombreUsuario());
    lblCreadoPara->setStyleSheet("font-size: 22px; font-weight: bold;");
    layoutScroll->addWidget(lblCreadoPara);

    // ==================== SECCION: Canciones ====================
    QLabel* lblCanciones=new QLabel("Canciones");
    lblCanciones->setStyleSheet("font-size: 20px; font-weight: bold;");
    layoutScroll->addWidget(lblCanciones);

    //Scroll horizontal
    QScrollArea* scrollCanciones=new QScrollArea();
    scrollCanciones->setWidgetResizable(true);
    scrollCanciones->setFixedHeight(180);
    scrollCanciones->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollCanciones->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollCanciones->setStyleSheet("border: none;");

    //Contenedor horizontal
    QWidget* contenedorCanciones=new QWidget();
    QHBoxLayout* layoutCanciones=new QHBoxLayout(contenedorCanciones);
    layoutCanciones->setSpacing(20);
    layoutCanciones->setContentsMargins(10,10,10,10);

    // Canciones (widgets en fila)
    for(int i=0;i<1;++i)
    {

        QLabel*cancion=new QLabel("🎵 Canción "+QString::number(i+1));
        cancion->setFixedSize(120, 120);
        cancion->setAlignment(Qt::AlignCenter);
        cancion->setStyleSheet("background-color: #222; border-radius: 10px; color: white;");
        layoutCanciones->addWidget(cancion);

    }

    scrollCanciones->setWidget(contenedorCanciones);
    layoutScroll->addWidget(scrollCanciones);

    // ==================== SECCION: EP ====================
    QLabel* lblEp =new QLabel("EP");
    lblEp->setStyleSheet("font-size: 20px; font-weight: bold;");
    layoutScroll->addWidget(lblEp);

    QScrollArea*scrollEp= new QScrollArea();
    scrollEp->setWidgetResizable(true);
    scrollEp->setFixedHeight(160);
    scrollEp->setStyleSheet("border: none;");
    scrollEp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollEp->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget*contenedorEp=new QWidget();
    QHBoxLayout*layoutEp=new QHBoxLayout(contenedorEp);
    layoutEp->setSpacing(20);
    layoutEp->setContentsMargins(10,10,10,10);

    // EPs
    for(int i=0;i<4;++i)
    {

        QLabel*ep=new QLabel("💿 EP " + QString::number(i+1));
        ep->setFixedSize(120, 120);
        ep->setAlignment(Qt::AlignCenter);
        ep->setStyleSheet("background-color: #222; border-radius: 10px; color: white;");
        layoutEp->addWidget(ep);

    }

    scrollEp->setWidget(contenedorEp);
    layoutScroll->addWidget(scrollEp);

    // ==================== SECCION: Albumes ====================
    QLabel*lblAlbums=new QLabel("Albums");
    lblAlbums->setStyleSheet("font-size: 20px; font-weight: bold;");
    layoutScroll->addWidget(lblAlbums);

    QScrollArea*scrollAlbums= new QScrollArea();
    scrollAlbums->setWidgetResizable(true);
    scrollAlbums->setFixedHeight(160);
    scrollAlbums->setStyleSheet("border: none;");
    scrollAlbums->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollAlbums->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* contenedorAlbums=new QWidget();
    QHBoxLayout* layoutAlbums=new QHBoxLayout(contenedorAlbums);
    layoutAlbums->setSpacing(20);
    layoutAlbums->setContentsMargins(10,10,10,10);

    // Álbumes
    for (int i = 0; i < 10; ++i)
    {
        QLabel* album = new QLabel("📀 Álbum " + QString::number(i+1));
        album->setFixedSize(120, 120);
        album->setAlignment(Qt::AlignCenter);
        album->setStyleSheet("background-color: #222; border-radius: 10px; color: white;");
        layoutAlbums->addWidget(album);
    }

    scrollAlbums->setWidget(contenedorAlbums);
    layoutScroll->addWidget(scrollAlbums);

    QLabel *lblArtistas =new QLabel("Artistas populares");
    lblArtistas->setStyleSheet("font-size: 20px; font-weight: bold;");
    layoutScroll->addWidget(lblArtistas);

//=====SCROLL ARTISTAS====================================================================================

    QScrollArea*scrollArtistas=new QScrollArea();
    scrollArtistas->setWidgetResizable(true);
    scrollArtistas->setFixedHeight(200);
    scrollArtistas->setStyleSheet("border: none;");
    scrollArtistas->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);//AGREGA EL SCROLL HORIZONTAL
    scrollArtistas->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//QUITA EL SCROLL VERTICAL
    scrollArtistas->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);//Asi se evitam que el widget crezca en vertical.

    QWidget*widgetArtistas=new QWidget();
    widgetArtistas->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
    QHBoxLayout*filaArtistas=new QHBoxLayout(widgetArtistas);
    filaArtistas->setSpacing(30);//SEPARACION ENTRE ARTISTAS
    filaArtistas->setContentsMargins(10,10,10,10);
    filaArtistas->setAlignment(Qt::AlignLeft);

    GestorArtistas gestor;
    QVector<Artista>artistas=gestor.leerArtista();
    QVector<Artista> artistasActivos;
    for(const Artista&a:artistas)
    {

        qDebug()<<"Artista:"<<a.getNombreArtistico()<<"Imagen:"<<a.getRutaImagen();

        artistasActivos.append(a);

    }

    if(artistasActivos.isEmpty())
    {

        QLabel*sinArtistas=new QLabel("No hay artistas registrados aun");
        sinArtistas->setStyleSheet("font-style: italic; font-size: 14px; color: gray;");
        layoutScroll->addWidget(sinArtistas);

    }else{

        for(const Artista&art:artistasActivos)
        {

            QVBoxLayout*col=new QVBoxLayout();
            col->setAlignment(Qt::AlignCenter);

            QLabel*foto=new QLabel();
            foto->setFixedSize(120,120);
            foto->setStyleSheet("border-radius: 60px; background-color: #222;");

            QPixmap pix(art.getRutaImagen());
            if(!pix.isNull())
            {

                int lado=qMin(pix.width(),pix.height());
                QRect centro((pix.width()-lado)/2,(pix.height()-lado)/2,lado,lado);
                QPixmap cuadrado=pix.copy(centro);
                QPixmap escalado=cuadrado.scaled(foto->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

                QPixmap circular(foto->size());
                circular.fill(Qt::transparent);

                QPainter painter(&circular);
                painter.setRenderHint(QPainter::Antialiasing);
                QPainterPath path;
                path.addEllipse(0,0,foto->width(),foto->height());
                painter.setClipPath(path);
                painter.drawPixmap(0,0,escalado);
                painter.end();

                foto->setPixmap(circular);

                //EFECTO SOMBRAAAAAA
                QGraphicsDropShadowEffect*sombra=new QGraphicsDropShadowEffect();
                sombra->setBlurRadius(20);
                sombra->setOffset(0,0);
                sombra->setColor(QColor(0,0,0,160));
                foto->setGraphicsEffect(sombra);

            }

            QLabel*nombreLbl=new QLabel(art.getNombreArtistico());
            nombreLbl->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
            nombreLbl->setAlignment(Qt::AlignCenter);

            QLabel*etiqueta=new QLabel("Artista");
            etiqueta->setStyleSheet("color: gray; font-size: 12px;");
            etiqueta->setAlignment(Qt::AlignCenter);

            col->addWidget(foto);
            col->addSpacing(5);
            col->addWidget(nombreLbl);
            col->addWidget(etiqueta);

            QWidget*artistaWidget=new QWidget();
            artistaWidget->setLayout(col);
            filaArtistas->addWidget(artistaWidget);

        }
        scrollArtistas->setWidget(widgetArtistas);
        // ---------- Fade lateral (sombra lateral derecha tipo Spotify) ----------
        QWidget*contenedorFade=new QWidget();
        contenedorFade->setFixedHeight(200);
        contenedorFade->setFixedWidth(50);
        contenedorFade->setAttribute(Qt::WA_TransparentForMouseEvents);//NO BLOQUEA EL SCROLL

        QPixmap fadePix(50,200);
        fadePix.fill(Qt::transparent);
        QPainter fadePainter(&fadePix);
        QLinearGradient gradiente(0,0,50,0);//DE IZQUIERDA A DERECHA
        gradiente.setColorAt(0,QColor(18,18,18,0));//TRANSPARENTE
        gradiente.setColorAt(1,QColor(18,18,18,255));//FONDO OSCURO OPACOOOO

        fadePainter.fillRect(fadePix.rect(),gradiente);
        fadePainter.end();

        QLabel*fadeLabel=new QLabel(contenedorFade);
        fadeLabel->setPixmap(fadePix);
        fadeLabel->setFixedSize(50,200);

        // ---------- Contenedor con layout absoluto para superponer fade sobre scroll ----------
        QWidget*contenedorScrollYFade=new QWidget();
        contenedorScrollYFade->setFixedHeight(200);
        QStackedLayout*stack=new QStackedLayout(contenedorScrollYFade);
        stack->addWidget(scrollArtistas);//SCROLL HORIZONTAL
        stack->addWidget(contenedorFade);//FADE QUE SE SUPERPONE
        stack->setStackingMode(QStackedLayout::StackAll);// Superpone el fade sobre el scroll

        // Agrega todo al scroll general vertical
        layoutScroll->addWidget(contenedorScrollYFade);

    }
    contenidoScroll->setLayout(layoutScroll);
    scroll->setWidget(contenidoScroll);

    colContenido->addWidget(scroll);

    QWidget*panelDerecho=new QWidget();
    panelDerecho->setLayout(colContenido);
    layoutColumnas->addWidget(panelDerecho);

}

void Home::Regresar()
{

    MenuInicio*m=new MenuInicio(nullptr);
    m->show();
    this->close();

}
