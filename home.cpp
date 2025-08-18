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
#include"perfilusuario.h"
#include<QMessageBox>
#include<QListWidget>
#include<QInputDialog>
#include<QDir>
#include<QRegularExpression>
#include<QFileInfo>
#include"vistaperfilartista.h"

Home::Home(const Usuario& usuarioActivo, QWidget *parent): QWidget(parent), usuario(usuarioActivo)
{
    setWindowTitle("Inicio");
    setFixedSize(1100,720);

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

    //BotonEliminar playlist seleccionada
    QPushButton*btnEliminar=new QPushButton("🗑");
    btnEliminar->setFixedSize(30,30);
    btnEliminar->setCursor(Qt::PointingHandCursor);
    btnEliminar->setToolTip("Eliminar playlist seleccionada");
    btnEliminar->setStyleSheet(R"(
        QPushButton {
            background-color: #E53935;  /* rojo */
            color: white;
            border: none;
            border-radius: 15px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #ef5350;
        }
        QPushButton:disabled {
            background-color: #444;
            color: #888;
        }
    )");

    layoutBibliotecaHeader->addWidget(btnEliminar);
    layoutBibliotecaHeader->addWidget(btnAgregar);

    colBiblioteca->addLayout(layoutBibliotecaHeader);

    listaPlaylists=new QListWidget();
    listaPlaylists->setStyleSheet(R"(
    QListWidget {
        background-color: #121212;
        color: white;
        border: none;
        padding: 6px;
    }
    QListWidget::item {
        background: #181818;
        border: 1px solid #333;
        border-radius: 14px;
        padding: 8px 6px 10px 6px; /* espacio para el texto bajo la portada */
        margin: 2px;
    }
    QListWidget::item:hover {
        border: 1px solid #555;
        background: #1c1c1c;
    }
    QListWidget::item:selected {
        border: 1px solid #1DB954;
        background: #1c1c1c;
        color: white;
    }
    )");
    colBiblioteca->addWidget(listaPlaylists);

    //Aqui placeholder de playlists
    lblVacio=new QLabel("No hay playlists aún");
    lblVacio->setStyleSheet("font-style: italic; font-size: 14px; color: gray;");
    colBiblioteca->addWidget(lblVacio);

    connect(btnAgregar,&QPushButton::clicked,this,[=](){

        crearPlaylist();

    });

    btnEliminar->setEnabled(false);
    connect(listaPlaylists, &QListWidget::currentRowChanged, this, [=](int row)
    {

        btnEliminar->setEnabled(row>=0);

    });

    connect(btnEliminar, &QPushButton::clicked, this, [=]()
    {

        auto* it=listaPlaylists->currentItem();
        if(!it)
        {

            QMessageBox::information(this, "Playlist", "Selecciona una playlist primero.");
            return;

        }
        eliminarPlaylistPorNombre(it->text());

    });

    cargarPlaylists();

    // Menu contextual para eliminar playlists
    listaPlaylists->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listaPlaylists, &QListWidget::customContextMenuRequested,this,[=](const QPoint& pos)
    {

        QListWidgetItem*it=listaPlaylists->itemAt(pos);
        if(!it)return;

        QMenu menu(this);
        QAction*actEliminar=menu.addAction("Eliminar playlist");
        QAction*chosen=menu.exec(listaPlaylists->viewport()->mapToGlobal(pos));
        if(chosen==actEliminar)
        {

            eliminarPlaylistPorNombre(it->text());

        }
    });

    connect(listaPlaylists, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem* it)
    {
        const QString baseDir="Publico";
        const QString carpetaUsuario=baseDir+"/Usuario_"+usuario.getNombreUsuario();
        const QString pathDat=carpetaUsuario+"/"+it->text()+".dat";

        //1. VALIDACIONESSSSZZ
        QFile f(pathDat);
        if(!f.exists())
        {

            QMessageBox::warning(this,"Playlist", "El archivo de la playlist no existe.");
            return;

        }
        if(QFileInfo(pathDat).size()==0)//EVITAR CRASH
        {

            QMessageBox::information(this, "Playlist vacia","Esta playlist no tiene canciones.");
            return;

        }

        // Leer canciones del .dat
        QVector<Cancion>cancionesLeidas;
        if(f.open(QIODevice::ReadOnly))
        {

            QDataStream in(&f);
            in.setVersion(QDataStream::Qt_5_15);
            while (!in.atEnd())
            {

                quint32 magic; quint16 ver;
                in>>magic>>ver;
                if(in.status()!=QDataStream::Ok)break;
                if(magic!=0x534F4E47||ver!=2) break; // 'SONG', v2

                quint32 id=0;
                if(ver>=2)
                {

                    //v2: viene el id primero
                    in>>id;

                }

                QString titulo,artista,desc,rutaAudio,rutaImagen,duracionStr;
                quint16 tipo,genero;
                QDate fecha;
                qint32 reproducciones;
                bool activo;

                in>>titulo;
                in>>artista;
                in>>tipo;
                in>>desc;
                in>>genero;
                in>>fecha;
                in>>duracionStr;
                in>>rutaAudio;
                in>>rutaImagen;
                in>>reproducciones;
                in>>activo;

                Cancion c;
                c.setID(static_cast<int>(id));
                c.setTitulo(titulo);
                c.setNombreArtista(artista);
                c.setTipo(static_cast<Tipo>(tipo));
                c.setDescripcion(desc);
                c.setGenero(static_cast<Genero>(genero));
                c.setFechaCarga(fecha);
                c.setDuracion(duracionStr);   // ya la guardamos como QString
                c.setRutaAudio(rutaAudio);
                c.setRutaImagen(rutaImagen);
                c.setReproducciones(reproducciones);
                c.setActiva(activo);

                cancionesLeidas.append(c);
            }
            f.close();
        }

        auto*rep=new ReproductorMusica(cancionesLeidas,usuario,nullptr,true,pathDat);
        rep->show();
        this->hide();

    });


    //Panel lateral izquierdo
    QWidget*panelIzquierdo=new QWidget();
    panelIzquierdo->setLayout(colBiblioteca);
    panelIzquierdo->setFixedWidth(260);
    panelIzquierdo->setStyleSheet("background-color: #121212; color: white; padding: 10px;");

    layoutColumnas->addWidget(panelIzquierdo);

    // ================== COLUMNA DERECHA ==================
    QVBoxLayout*colContenido=new QVBoxLayout();

    // ---------- BARRA SUPERIOR ----------
    QHBoxLayout*barraSuperior=new QHBoxLayout();

    QLabel*lblHome=new QLabel("🏠");
    lblHome->setStyleSheet("font-size: 22px;");
    //barraSuperior->addWidget(lblHome);

    QLineEdit *busqueda=new QLineEdit();
    busqueda->setPlaceholderText("¿Qué quieres reproducir?");
    busqueda->setFixedHeight(30);
    busqueda->setStyleSheet("border-radius: 15px; padding-left: 10px;");
    //barraSuperior->addWidget(busqueda,1);

    barraSuperior->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    QPushButton*bntImagen=new QPushButton();
    bntImagen->setFixedSize(50,50);
    bntImagen->setStyleSheet(

        "border-radius: 20px;"
        "border: 2px solid white;"
        "background-color: #222;"

    );
    bntImagen->setCursor(Qt::PointingHandCursor);

    //Aqui imagen circular personalizada
    QPixmap pix(usuario.getRutaFoto());
    if(!pix.isNull())
    {

        int lado=qMin(pix.width(),pix.height());
        QRect centro((pix.width()-lado)/2,(pix.height()-lado)/2,lado,lado);
        QPixmap cuadrado =pix.copy(centro);
        QPixmap escalado= cuadrado.scaled(bntImagen->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QPixmap circular(bntImagen->size());
        circular.fill(Qt::transparent);

        QPainter painter(&circular);
        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addEllipse(0,0,bntImagen->width(), bntImagen->height());
        painter.setClipPath(path);
        painter.drawPixmap(0,0,escalado);
        painter.end();

        bntImagen->setIcon(circular);
        bntImagen->setIconSize(bntImagen->size());

        //EFECTO SOMBRAAAAAAA VERDECITOUUUUU
        QGraphicsDropShadowEffect*sombra=new QGraphicsDropShadowEffect(this);
        sombra->setBlurRadius(15);
        sombra->setOffset(0,0);
        sombra->setColor(QColor("#1DB954"));
        bntImagen->setGraphicsEffect(sombra);

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

    connect(bntImagen,&QPushButton::clicked,this,[=](){

        auto p=new PerfilUsuario(usuarioActivo,nullptr);
        p->show();
        this->hide();

    });

    connect(btnVolver, &QPushButton::clicked, this, &Home::Regresar);
    barraSuperior->addSpacing(20);
    barraSuperior->addWidget(btnVolver);
    barraSuperior->addSpacing(20);

    QMenu *menuPerfil = new QMenu();
    menuPerfil->addAction("Perfil");
    bntImagen->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(bntImagen, &QLabel::customContextMenuRequested, [=](const QPoint &pos) {
        menuPerfil->exec(bntImagen->mapToGlobal(pos));
    });

    barraSuperior->addWidget(bntImagen);
    colContenido->addLayout(barraSuperior);

    // ---------- AREA DE SCROLL ----------
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    QWidget *contenidoScroll = new QWidget();
    QVBoxLayout *layoutScroll = new QVBoxLayout(contenidoScroll);

    QLabel *lblCreadoPara = new QLabel("Creado para " + usuario.getNombreUsuario());
    lblCreadoPara->setStyleSheet("font-size: 22px; font-weight: bold;");
    layoutScroll->addWidget(lblCreadoPara);

    // ==================== SECCION: Canciones(SINGLE) ====================
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
    layoutCanciones->setAlignment(Qt::AlignLeft);

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
            btnImagen->setFixedSize(110,110);
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

            QObject::connect(btnImagen,&QPushButton::clicked,this,[=]()
            {

                QVector<Cancion>unicaCancion={c};//SOLO 1 CANCIONNNNZ
                ReproductorMusica*r=new ReproductorMusica(unicaCancion,usuarioActivo,nullptr,false,"");
                r->show();
                this->hide();

            });

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
    layoutEp->setAlignment(Qt::AlignLeft);

    // EPs
    for(auto i=epsAgrupados.begin();i!=epsAgrupados.end();++i)
    {

        const QVector<Cancion>& epCanciones=i.value();
        if(epCanciones.size()>=1)
        {

            QFrame*contenedorItem=new QFrame();
            contenedorItem->setFixedWidth(130);
            contenedorItem->setFrameShape(QFrame::Box);
            contenedorItem->setStyleSheet(R"(
              QFrame {
                    border: 2px solid transparent;
                    border-radius: 12px;
                    background-color: #111;
                }
                QFrame:hover {
                    border: 2px solid #ff3333;
                    background-color: #181818;
                }
            )");

            QVBoxLayout*layoutItem=new QVBoxLayout(contenedorItem);
            layoutItem->setSpacing(5);
            layoutItem->setAlignment(Qt::AlignCenter);

            QPushButton*btnImagen=new QPushButton();
            btnImagen->setFixedSize(110,110);
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

            QObject::connect(btnImagen,&QPushButton::clicked,this,[=](){

                ReproductorMusica*r=new ReproductorMusica(epCanciones,usuarioActivo,nullptr,false,"");
                r->show();
                this->hide();

            });

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
    layoutAlbums->setAlignment(Qt::AlignLeft);

    //Mostrar albumes validos (minimo 8 canciones activas por artista)
    for(auto i=AlbumAgrupados.begin();i!=AlbumAgrupados.end();++i)
    {

        const QVector<Cancion>& albumCanciones=i.value();
        if(albumCanciones.size()>=1)
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
            btnImagen->setFixedSize(110, 110);
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

            QObject::connect(btnImagen,&QPushButton::clicked,this,[=](){

                ReproductorMusica*r=new ReproductorMusica(albumCanciones,usuarioActivo,nullptr,false,"") ;
                r->show();
                this->hide();

            });

        }

    }

    scrollAlbums->setWidget(contenedorAlbums);
    layoutScroll->addWidget(scrollAlbums);

//==========TERMINA AREA SCROLL==========================================================================

//=====SCROLL ARTISTAS====================================================================================
    QLabel *lblArtistas =new QLabel("Artistas");
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

            QPushButton*foto=new QPushButton();
            foto->setObjectName("fotoArtista");
            foto->setFixedSize(120,120);
            foto->setFlat(true);//SIN BORDES
            foto->setCursor(Qt::PointingHandCursor);
            foto->setStyleSheet(
                "#fotoArtista { border: none; border-radius: 60px; background-color: #222; }"
                "#fotoArtista:focus { outline: none; }"
                );

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

                foto->setIcon(QIcon(circular));
                foto->setIconSize(foto->size());

                //EFECTO SOMBRAAAAAA
                auto*sombra=new QGraphicsDropShadowEffect();
                sombra->setBlurRadius(20);
                sombra->setOffset(0,0);
                sombra->setColor(QColor(0,0,0,160));
                foto->setGraphicsEffect(sombra);

            }

            foto->setToolTip(art.getNombreArtistico());
            //CONNECT DEL BOTON
            connect(foto,&QPushButton::clicked,this,[=](){

                auto*v=new VisitaPerfilArtista(art,usuarioActivo,nullptr);
                v->show();
                this->hide();

            });

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

    // ==================== SECCION: Genero ====================
    QLabel*lblGenero=new QLabel("Género");
    lblGenero->setStyleSheet("font-size: 22px; font-weight: bold;");
    layoutScroll->addWidget(lblGenero);

    // Contenedor y grid
    QWidget*generoWrap=new QWidget();
    QGridLayout*gridGenero=new QGridLayout(generoWrap);
    gridGenero->setSpacing(18);
    gridGenero->setContentsMargins(0,6,0,6);

    struct GenItem{QString nombre;QString color; Genero g;};
    const QVector<GenItem>generos=
    {

        {"Pop","#2ebd59",Genero::Pop},
        {"Corridos","#e91e63",Genero::Corridos},
        {"Cristianos","#3f51b5",Genero::Cristianos},
        {"Reguetón","#9c27b0",Genero::Regueton},     // enum es Regueton
        {"Electrónica","#009688",Genero::Electronica},  // enum es Electronica
        {"Rock","#ff5722", Genero::Rock},
        {"Clásicas","#607d8b",Genero::Clasicas}

    };

    // Crea cada “tarjeta” de genero
    auto crearTileGenero = [&](const GenItem& gi)->QPushButton*
    {

        QPushButton* tile = new QPushButton(gi.nombre);
        tile->setCursor(Qt::PointingHandCursor);
        tile->setMinimumSize(320, 150);
        tile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        tile->setStyleSheet(QString(
                                "QPushButton {"
                                "  background-color: %1;"
                                "  color: white;"
                                "  border: none;"
                                "  border-radius: 14px;"
                                "  font-weight: 800;"
                                "  font-size: 20px;"
                                "  text-align: left;"
                                "  padding: 14px 16px;"
                                "}"
                                "QPushButton:hover {"
                                "  background-color: %2;"
                                "}"
                                ).arg(gi.color, QColor(gi.color).lighter(110).name()));
        // Click -> abrir reproductor con canciones de ese genero
        connect(tile,&QPushButton::clicked,this,[=]()
        {

            AbrirPorGenero(gi.g);

        });
        return tile;

    };

    // Distribucion en una grilla 3xN (7 elementos total)
    for(int i = 0; i < generos.size(); ++i)
    {

        int r = i / 3;           // 3 columnas
        int c = i % 3;
        gridGenero->addWidget(crearTileGenero(generos[i]), r, c);

    }

    layoutScroll->addWidget(generoWrap);

    contenidoScroll->setLayout(layoutScroll);
    scroll->setWidget(contenidoScroll);

    colContenido->addWidget(scroll);

    QWidget*panelDerecho=new QWidget();
    panelDerecho->setLayout(colContenido);
    layoutColumnas->addWidget(panelDerecho);

}

void Home::Regresar()
{

    //NOTA:this como padre -> provoca que se cierre tambien el hijo, el nullptr no se mantiene como padre
    MenuInicio*m=new MenuInicio(nullptr);
    m->show();
    this->close();

}

void Home::AbrirPorGenero(Genero g)
{

    GestorCanciones gestor;
    const QVector<Cancion>todas=gestor.leerCanciones();

    QVector<Cancion>filtradas;
    filtradas.reserve(todas.size());
    for(const Cancion&c:todas)
    {

        if(c.estaActiva()&&c.getGenero()==g)
        {

            filtradas.append(c);

        }

    }
    if(filtradas.isEmpty())
    {

        QMessageBox::information(this,"Genero","No hay canciones para este genero todavia.");
        return;

    }
    auto*rep=new ReproductorMusica(filtradas,usuario,nullptr,false,"");
    rep->show();
    this->hide();

}

void Home::cargarPlaylists()
{

    listaPlaylists->clear();

    const QString baseDir="Publico";
    const QString carpetaUsuario=baseDir+"/Usuario_"+usuario.getNombreUsuario();

    QDir dir(carpetaUsuario);
    if(!dir.exists())
    {

        //SI NO EXISTE AUN, POR AL AZAR DEL DESTINOOOOO, no se muestra nada
        lblVacio->show();
        return;


    }
    //LISTAR SOLO ARCHIVOS.DAT, CADA UNA ES UNA PLAYLIST
    QStringList subDirs=dir.entryList(QStringList()<<"*.dat",QDir::Files,QDir::Name);
    for(const QString&archivo:subDirs)
    {

        //Mostrar sin la extension .dat
        QString nombreSinExtension=QFileInfo(archivo).completeBaseName();
        listaPlaylists->addItem(nombreSinExtension);

    }

    //MUESTRA/OCULTA PLACEHOLDER
    lblVacio->setVisible(listaPlaylists->count()==0);

}


void Home::crearPlaylist()
{

    bool ok=false;
    QString nombre=QInputDialog::getText(this,"Nueva playList","Escribe un nombre:",QLineEdit::Normal,"",&ok).trimmed();
    if(!ok||nombre.isEmpty())return;

    //SANEA NOMBRE BASICO PARA CARPETAS
    nombre.replace(QRegularExpression(R"([\/:*?"<>|])"),"_");

    //EVITAR ESPACIOS O PUNTOS AL FINAL
    while(nombre.endsWith(' ')||nombre.endsWith('.'))
    {

        nombre.chop(1);

    }

    if(nombre.isEmpty())
    {

        QMessageBox::warning(this,"PlayList","El nombre no puede quedar vacio.");
        return;

    }
    //RUTAS
    const QString baseDir="Publico";
    const QString carpetaUsuario=baseDir+"/Usuario_"+usuario.getNombreUsuario();
    const QString ArchivoPlayList=carpetaUsuario+"/"+nombre+".dat";

    QDir dir;

    //ASEGURAR BASE Y CARPETA DEL USUARIO
    if(!dir.exists(baseDir))dir.mkpath(baseDir);
    if(!dir.exists(carpetaUsuario))dir.mkpath(carpetaUsuario);

    //EVITAR DUPLICADOS
    if(QFile::exists(ArchivoPlayList))
    {

        QMessageBox::warning(this,"PlayList","Ya existe una playList con ese nombre.");
        return;

    }
    //CREAR ARCHIVO BINARIO VACIO DE LA PLAYLIST
    QFile archivo(ArchivoPlayList);
    if(!archivo.open(QIODevice::WriteOnly))
    {

        QMessageBox::critical(this, "PlayList", "No se pudo crear el archivo de la playlist");
        return;

    }
    archivo.close();

    //EXITOOOOO, SE REFLEJA EN EL FRONTED
    listaPlaylists->addItem(nombre);
    lblVacio->setVisible(false);

}

bool Home::playlistTieneCanciones(const QString &pathDat) const
{

    //si el archivo pesa>0, hay canciones
    QFileInfo fi(pathDat);
    return fi.exists()&&fi.size()>0;

}

void Home::eliminarPlaylistPorNombre(const QString &nombre)
{

    const QString baseDir="Publico";
    const QString carpetaUsuario=baseDir+"/Usuario_"+usuario.getNombreUsuario();
    const QString pathDat=carpetaUsuario+"/"+nombre+".dat";
    const QString pathIdx=pathDat+".idx";

    if(!QFile::exists(pathDat))
    {

        QMessageBox::warning(this, "Playlist", "El archivo de la playlist no existe.");
        return;

    }

    //Tiene canciones?
    const bool tieneCanciones=playlistTieneCanciones(pathDat);


    if(tieneCanciones)
    {

        const auto r=QMessageBox::question(this,"Eliminar playlist","Esta playlist tiene canciones.\n""¿Estás seguro de eliminarla?",QMessageBox::Yes | QMessageBox::No);
        if(r!=QMessageBox::Yes)return;

    }
    //Si no tiene canciones, se elimina de un solo

    //Borrar archivos (.dat y .idx si existe)
    bool okDat=QFile::remove(pathDat);
    bool okIdx=true;
    if(QFile::exists(pathIdx))okIdx=QFile::remove(pathIdx);

    if(!okDat||!okIdx)
    {

        QMessageBox::critical(this, "Playlist", "No se pudo eliminar uno o mas archivos de la playlist.");
        return;

    }

    // Quitar de la UI
    QList<QListWidgetItem*>items=listaPlaylists->findItems(nombre,Qt::MatchExactly);
    if(!items.isEmpty())
    {

        delete listaPlaylists->takeItem(listaPlaylists->row(items.first()));

    }

    // Mostrar/ocultar placeholder
    lblVacio->setVisible(listaPlaylists->count()==0);

    QMessageBox::information(this, "Playlist", "Playlist eliminada.");

}
