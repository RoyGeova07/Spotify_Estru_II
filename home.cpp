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
#include"cancion.h"
#include"gestorcanciones.h"

Home::Home(const Usuario& usuarioActivo, QWidget *parent): QWidget(parent), usuario(usuarioActivo)
{
    setWindowTitle("Inicio");
    setFixedSize(1100, 700);

    QHBoxLayout*layoutColumnas=new QHBoxLayout(this);  //Columnaa principal

    GestorCanciones gestorCanciones;
    QVector<Cancion>todasCanciones=gestorCanciones.leerCanciones();

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
    scrollCanciones->setFixedHeight(225);//TAMANIO DEL SCROLL DE SINGLE
    scrollCanciones->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollCanciones->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollCanciones->setStyleSheet("border: none;");

    //Contenedor horizontal
    QWidget* contenedorCanciones=new QWidget();
    QHBoxLayout* layoutCanciones=new QHBoxLayout(contenedorCanciones);
    layoutCanciones->setSpacing(20);
    layoutCanciones->setContentsMargins(10,10,10,10);

    //-------Canciones tipo SINGLE--------
    for(const Cancion& c:todasCanciones)
    {

        qDebug()<<"Artista"<<c.getNombreArtista();
        if(c.getTipo()==Tipo::Single&&c.estaActiva())
        {

            QFrame*contenedorItem=new QFrame();
            contenedorItem->setFixedWidth(130);
            contenedorItem->setFrameShape(QFrame::Box);
            contenedorItem->setStyleSheet(R"(
                QFrame {
                    border: 2px solid transparent;
                    border-radius: 10px;
                    background-color: #111;
                }
                QFrame:hover {
                    border: 2px solid #ff3333;
                }
            )");
            QVBoxLayout*layoutItem=new QVBoxLayout(contenedorItem);
            layoutItem->setSpacing(5);
            layoutItem->setAlignment(Qt::AlignCenter);

            QPushButton*btnImagen=new QPushButton();
            btnImagen->setFixedSize(130,130);
            btnImagen->setCursor(Qt::PointingHandCursor);
            btnImagen->setStyleSheet(R"(
                QPushButton {
                    border: none;
                    background-color: #222;
                }
            )");

            QPixmap pix(c.getRutaImagen());
            if(!pix.isNull())
            {

                QPixmap scaled=pix.scaled(btnImagen->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);
                btnImagen->setIcon(scaled);
                btnImagen->setIconSize(btnImagen->size());

            }else{

                btnImagen->setText("Sin imagen");
                btnImagen->setStyleSheet("color: white; background-color: #800; border-radius: 10px;");

            }

            QLabel*lblTitulo=new QLabel(c.getTitulo());
            lblTitulo->setStyleSheet("color: white; font-weight: bold;");
            lblTitulo->setAlignment(Qt::AlignCenter);
            lblTitulo->setWordWrap(true);//se utiliza para habilitar o deshabilitar el ajuste de linea de texto en un widget de etiqueta (QLabel).

            QLabel*lblArtista=new QLabel(c.getNombreArtista());
            lblArtista->setStyleSheet("color: gray; font-size: 12px;");
            lblArtista->setAlignment(Qt::AlignCenter);
            lblArtista->setWordWrap(true);

            layoutItem->addWidget(btnImagen);
            layoutItem->addWidget(lblTitulo);
            layoutItem->addWidget(lblArtista);

            layoutCanciones->addWidget(contenedorItem);

        }

    }

    scrollCanciones->setWidget(contenedorCanciones);
    layoutScroll->addWidget(scrollCanciones);

    // ==================== SECCION: EP ====================
    QMap<QString, QVector<Cancion>> epsAgrupados;

    for(const Cancion& c:todasCanciones)
    {

        if(c.getTipo()==Tipo::EP&&c.estaActiva())
        {

            epsAgrupados[c.getNombreArtista()].append(c);

        }

    }

    QLabel* lblEp =new QLabel("EP");
    lblEp->setStyleSheet("font-size: 20px; font-weight: bold;");
    layoutScroll->addWidget(lblEp);

    QScrollArea*scrollEp= new QScrollArea();
    scrollEp->setWidgetResizable(true);
    scrollEp->setFixedHeight(220);
    scrollEp->setStyleSheet("border: none;");
    scrollEp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollEp->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget*contenedorEp=new QWidget();
    QHBoxLayout*layoutEp=new QHBoxLayout(contenedorEp);
    layoutEp->setSpacing(20);
    layoutEp->setContentsMargins(10,10,10,10);

    // EPs
    for(auto i=epsAgrupados.begin();i!=epsAgrupados.end();++i)
    {

        const QVector<Cancion>& epCanciones=i.value();
        if(epCanciones.size()>=3)//EL EP ES VALIDO CON AL MENOS 3 CANCIONES
        {

            QFrame*contenedorItem=new QFrame();
            contenedorItem->setFixedWidth(130);
            contenedorItem->setFrameShape(QFrame::Box);
            contenedorItem->setStyleSheet(R"(
                QFrame {
                    border: 2px solid transparent;
                    border-radius: 10px;
                    background-color: #111;
                }
                QFrame:hover {
                    border: 2px solid #ff3333;
                }
            )");

            QVBoxLayout*layoutItem=new QVBoxLayout(contenedorItem);
            layoutItem->setSpacing(5);
            layoutItem->setAlignment(Qt::AlignCenter);

            QPushButton*btnImagen=new QPushButton();
            btnImagen->setFixedSize(130,130);
            btnImagen->setCursor(Qt::PointingHandCursor);
            btnImagen->setStyleSheet(R"(
                QPushButton {
                    border: none;
                    background-color: #222;
                }
            )");

            //Se usa la imagen de la primera cancion como portada representativa
            QPixmap pix(epCanciones.first().getRutaImagen());
            if(!pix.isNull())
            {

                QPixmap scaled=pix.scaled(btnImagen->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
                btnImagen->setIcon(scaled);
                btnImagen->setIconSize(btnImagen->size());

            }else{

                btnImagen->setText("Sin imagen");
                btnImagen->setStyleSheet("color: white; background-color: #800; border-radius: 10px;");

            }

            QLabel*lblTitulo=new QLabel("EP de "+epCanciones.first().getNombreArtista());
            lblTitulo->setStyleSheet("color: white; font-weight: bold;");
            lblTitulo->setAlignment(Qt::AlignCenter);
            lblTitulo->setWordWrap(true);

            layoutItem->addWidget(btnImagen);
            layoutItem->addWidget(lblTitulo);
            layoutEp->addWidget(contenedorItem);

            //CONNECT FUTURO AQUI

        }

    }

    scrollEp->setWidget(contenedorEp);
    layoutScroll->addWidget(scrollEp);

    // ==================== SECCION: Albumes ====================
    QMap<QString, QVector<Cancion>> AlbumAgrupados;

    for(const Cancion&c:todasCanciones)
    {

        if(c.getTipo()==Tipo::Album&&c.estaActiva())
        {

            AlbumAgrupados[c.getNombreArtista()].append(c);

        }

    }

    QLabel*lblAlbums=new QLabel("Albums");
    lblAlbums->setStyleSheet("font-size: 20px; font-weight: bold;");
    layoutScroll->addWidget(lblAlbums);

    QScrollArea*scrollAlbums= new QScrollArea();
    scrollAlbums->setWidgetResizable(true);
    scrollAlbums->setFixedHeight(220);
    scrollAlbums->setStyleSheet("border: none;");
    scrollAlbums->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollAlbums->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* contenedorAlbums=new QWidget();
    QHBoxLayout* layoutAlbums=new QHBoxLayout(contenedorAlbums);
    layoutAlbums->setSpacing(20);
    layoutAlbums->setContentsMargins(10,10,10,10);

    //Mostrar albumes validos (minimo 8 canciones activas por artista)
    for(auto i=AlbumAgrupados.begin();i!=AlbumAgrupados.end();++i)
    {

        const QVector<Cancion>& albumCanciones=i.value();
        if(albumCanciones.size()>=8)//minimo 8 canciones por album
        {

            QFrame*contenedorItem=new QFrame();
            contenedorItem->setFixedWidth(130);
            contenedorItem->setFrameShape(QFrame::Box);
            contenedorItem->setStyleSheet(R"(
                QFrame {
                    border: 2px solid transparent;
                    border-radius: 10px;
                    background-color: #111;
                }
                QFrame:hover {
                    border: 2px solid #ff3333;
                }
            )");

            QVBoxLayout*layoutItem= new QVBoxLayout(contenedorItem);
            layoutItem->setSpacing(5);
            layoutItem->setAlignment(Qt::AlignCenter);

            QPushButton*btnImagen = new QPushButton();
            btnImagen->setFixedSize(130, 130);
            btnImagen->setCursor(Qt::PointingHandCursor);
            btnImagen->setStyleSheet(R"(
                QPushButton {
                    border: none;
                    background-color: #222;
                }
            )");

            //Imagen representativa del Album = la de la primera cancion
            QPixmap pix(albumCanciones.first().getRutaImagen());
            if(!pix.isNull())
            {

                QPixmap scaled =pix.scaled(btnImagen->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
                btnImagen->setIcon(scaled);
                btnImagen->setIconSize(btnImagen->size());

            }else{

                btnImagen->setText("Sin imagen");
                btnImagen->setStyleSheet("color: white; background-color: #800; border-radius: 10px;");

            }

            QLabel*lblTitulo =new QLabel("Album de "+albumCanciones.first().getNombreArtista());
            lblTitulo->setStyleSheet("color: white; font-weight: bold;");
            lblTitulo->setAlignment(Qt::AlignCenter);
            lblTitulo->setWordWrap(true);

            layoutItem->addWidget(btnImagen);
            layoutItem->addWidget(lblTitulo);
            layoutAlbums->addWidget(contenedorItem);

        }

    }

    scrollAlbums->setWidget(contenedorAlbums);
    layoutScroll->addWidget(scrollAlbums);

//==========TERMINA AREA SCROLL==========================================================================

//=====SCROLL ARTISTAS====================================================================================
    QLabel *lblArtistas =new QLabel("Artistas populares");
    lblArtistas->setStyleSheet("font-size: 20px; font-weight: bold;");
    layoutScroll->addWidget(lblArtistas);

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
