#include "vistaperfilartista.h"
#include<QPainter>
#include<QPainterPath>
#include<QGraphicsDropShadowEffect>
#include<QStackedLayout>
#include<QMessageBox>
#include"home.h"

VisitaPerfilArtista::VisitaPerfilArtista(const Artista&a,const Usuario&usuarioActual,QWidget*parent):QWidget(parent),artista(a),usuario(usuarioActual)
{

    setWindowTitle("Perfil de "+artista.getNombreArtistico());
    setFixedSize(1100,700);
    setStyleSheet("background-color: #121212; color: white;");

    construirUI();

}

void VisitaPerfilArtista::construirUI()
{

    auto*layoutPrincipal=new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(0,0,0,0);
    layoutPrincipal->setSpacing(0);

    // ---- Barra superior (Volver) ----
    auto*barra=new QHBoxLayout();
    barra->setContentsMargins(12,12,12,12);

    auto*btnVolver=new QPushButton("Volver");
    btnVolver->setCursor(Qt::PointingHandCursor);
    btnVolver->setStyleSheet(
        "QPushButton { background-color:#282828; color:white; border:1px solid white; "
        "border-radius:10px; padding:6px 12px; font-weight:bold; }"
        "QPushButton:hover { background-color:#3e3e3e; }"
        );
    connect(btnVolver, &QPushButton::clicked,this,&VisitaPerfilArtista::VolverHome);

    barra->addWidget(btnVolver);
    barra->addStretch();
    layoutPrincipal->addLayout(barra);

    // ---- Scroll vertical general ----
    auto*scroll=new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; }");

    auto*contenido=new QWidget();
    root=new QVBoxLayout(contenido);
    root->setContentsMargins(16,12,16,16);
    root->setSpacing(18);

    // ================= ENCABEZADO (caratula + datos) =================
    auto*header=new QHBoxLayout();
    header->setSpacing(20);

    lblFoto=new QLabel();
    lblFoto->setFixedSize(160,160);
    lblFoto->setStyleSheet("border-radius:80px; background:#222;");

    auto*sombra=new QGraphicsDropShadowEffect(this);
    sombra->setBlurRadius(20);
    sombra->setOffset(0,0);
    sombra->setColor(QColor("#1DB954"));
    lblFoto->setGraphicsEffect(sombra);

    QPixmap pixArt(artista.getRutaImagen());
    if (!pixArt.isNull())
        lblFoto->setPixmap(circularPixmap(pixArt, lblFoto->size()));

    auto*datos=new QVBoxLayout();
    datos->setSpacing(4);

    lblNombre=new QLabel(artista.getNombreArtistico());
    lblNombre->setStyleSheet("font-size:26px; font-weight:700; color:#1DB954;");

    lblPais=new QLabel("Pais: "+(artista.getPais().isEmpty() ? "N/D" : artista.getPais()));
    lblPais->setStyleSheet("color:gray;");

    lblRegistro=new QLabel("Registro: "+artista.getFechaRegistro().toString("dd/MM/yyyy"));
    lblRegistro->setStyleSheet("color:gray;");

    lblGeneroMusical=new QLabel("Genero musical: "+artista.getGenero());
    lblGeneroMusical->setStyleSheet("color:gray;");

    lblNacimiento=new QLabel("Nacimiento: "+artista.getFechaNacimiento().toString("dd/MM/yyyy"));
    lblNacimiento->setStyleSheet("color:gray;");

    lblDescripcion=new QLabel("Biografia: "+artista.getBiografia());
    lblDescripcion->setWordWrap(true);
    lblDescripcion->setStyleSheet("color:gray;");


    datos->addWidget(lblNombre);
    datos->addWidget(lblPais);
    datos->addWidget(lblRegistro);
    datos->addWidget(lblGeneroMusical);
    datos->addWidget(lblNacimiento);
    datos->addWidget(lblDescripcion);

    header->addWidget(lblFoto);
    header->addLayout(datos);
    header->addStretch();

    root->addLayout(header);
    QLabel*tituloMusicas=new QLabel(QStringLiteral("Musicas de %1").arg(artista.getNombreArtistico()));

    tituloMusicas->setStyleSheet(
        "font-size:24px; font-weight:800; color:#ffffff; "
        "padding:8px 0 0 0;"
        );

    root->addWidget(tituloMusicas);

    // ================= CARGA DE CANCIONES DEL ARTISTA =================
    QVector<Cancion>singles;
    QMap<QString, QVector<Cancion>>eps;
    QMap<QString, QVector<Cancion>>albums;
    cargarCancionesDelArtista(singles,eps,albums);

    // =================== SINGLES ===================
    root->addWidget(crearFilaSingles(singles, "Singles"));

    // =================== EP ===================
    {
        QFrame*cardEp=new QFrame();
        cardEp->setObjectName("SectionCard");
        cardEp->setStyleSheet(
            "#SectionCard { background-color:#151515; border:1px solid #2a2a2a; border-radius:16px; }"
            "#SectionCard QLabel[role='title']{ font-size:20px; font-weight:700; color:#fff; }"
            "#SectionCard QScrollBar:horizontal{ height:10px; background:#0f0f0f; }"
            "#SectionCard QScrollBar::handle:horizontal{ background:#1DB954; border-radius:5px; }"
            "#SectionCard QScrollBar::add-line:horizontal,"
            "#SectionCard QScrollBar::sub-line:horizontal{ width:0px; background:transparent; }"
            );
        QVBoxLayout*v=new QVBoxLayout(cardEp);
        v->setContentsMargins(14,12,14,12);
        v->setSpacing(8);

        QLabel*lblEp=new QLabel("EP");
        lblEp->setProperty("role","title");
        v->addWidget(lblEp);

        int epValidos=0;
        for(auto it=eps.begin();it!=eps.end();++it)
            if(it.value().size()>=3) ++epValidos;

        if(epValidos==0)
        {

            QLabel*vacio=new QLabel("Este artista aún no tiene EP");
            vacio->setStyleSheet("color:#a7a7a7;");
            vacio->setAlignment(Qt::AlignLeft);
            v->addWidget(vacio);

        }else{

            QScrollArea*scrollEp=new QScrollArea();
            scrollEp->setWidgetResizable(true);
            scrollEp->setFixedHeight(250);
            scrollEp->setStyleSheet("QScrollArea{ border:none; background:transparent; }");
            scrollEp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            scrollEp->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            QWidget*contenedorEp=new QWidget();
            QHBoxLayout*layoutEp=new QHBoxLayout(contenedorEp);
            layoutEp->setSpacing(18);
            layoutEp->setContentsMargins(6,10,6,10);
            layoutEp->setAlignment(Qt::AlignLeft);

            for(auto it=eps.begin();it!=eps.end();++it)
            {

                const QVector<Cancion>& epCanciones=it.value();
                if(epCanciones.size()<3)continue;

                QFrame*item=new QFrame();
                item->setFixedWidth(150);
                item->setStyleSheet(
                    "QFrame { background:#1a1a1a; border:1px solid #2b2b2b; border-radius:12px; }"
                    "QFrame:hover { background:#202020; border-color:#1DB954; }"
                    );
                QVBoxLayout*vb=new QVBoxLayout(item);
                vb->setSpacing(8);
                vb->setAlignment(Qt::AlignCenter);

                QPushButton*btn=new QPushButton();
                btn->setFixedSize(140,140);
                btn->setCursor(Qt::PointingHandCursor);
                btn->setStyleSheet(
                    "QPushButton{ border:none; background:#222; border-radius:10px; }"
                    "QPushButton:hover{ border:2px solid #1DB954; }"
                    );
                QPixmap pix(epCanciones.first().getRutaImagen());
                if(!pix.isNull())
                {

                    QPixmap scaled=pix.scaled(btn->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    btn->setIcon(scaled);
                    btn->setIconSize(btn->size());

                }else{

                    btn->setText("Sin imagen");
                    btn->setStyleSheet(btn->styleSheet()+" QPushButton{ color:white; background:#800; }");

                }

                QLabel*lbl=new QLabel("EP de "+epCanciones.first().getNombreArtista());
                lbl->setStyleSheet("color:#fff; font-weight:700;");
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setWordWrap(true);

                vb->addWidget(btn);
                vb->addWidget(lbl);
                layoutEp->addWidget(item);

                // (opcional) abrir reproductor del grupo
                // connect(btn, &QPushButton::clicked, this, [=](){
                //     auto* r = new ReproductorMusica(epCanciones, usuario, nullptr, false, "");
                //     r->show();
                //     this->hide();
                // });
            }

            scrollEp->setWidget(contenedorEp);
            v->addWidget(scrollEp);
        }

        root->addWidget(cardEp);
    }

    // =================== ALBUMES ===================
    {
        QFrame*cardAlbums=new QFrame();
        cardAlbums->setObjectName("SectionCard");
        cardAlbums->setStyleSheet(
            "#SectionCard { background-color:#151515; border:1px solid #2a2a2a; border-radius:16px; }"
            "#SectionCard QLabel[role='title']{ font-size:20px; font-weight:700; color:#fff; }"
            "#SectionCard QScrollBar:horizontal{ height:10px; background:#0f0f0f; }"
            "#SectionCard QScrollBar::handle:horizontal{ background:#1DB954; border-radius:5px; }"
            "#SectionCard QScrollBar::add-line:horizontal,"
            "#SectionCard QScrollBar::sub-line:horizontal{ width:0px; background:transparent; }"
            );
        QVBoxLayout*v=new QVBoxLayout(cardAlbums);
        v->setContentsMargins(14,12,14,12);
        v->setSpacing(8);

        QLabel*lblAlbums=new QLabel("Albums");
        lblAlbums->setProperty("role","title");
        v->addWidget(lblAlbums);

        int albumsValidos=0;
        for(auto it=albums.begin();it!=albums.end();++it)
            if(it.value().size()>=8)++albumsValidos;

        if(albumsValidos==0)
        {

            QLabel*vacio=new QLabel("Este artista aún no tiene albums");
            vacio->setStyleSheet("color:#a7a7a7;");
            vacio->setAlignment(Qt::AlignLeft);
            v->addWidget(vacio);

        }else{

            QScrollArea*scrollAlbums=new QScrollArea();
            scrollAlbums->setWidgetResizable(true);
            scrollAlbums->setFixedHeight(250);
            scrollAlbums->setStyleSheet("QScrollArea{ border:none; background:transparent; }");
            scrollAlbums->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            scrollAlbums->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            QWidget*contenedorAlbums=new QWidget();
            QHBoxLayout*layoutAlbums=new QHBoxLayout(contenedorAlbums);
            layoutAlbums->setSpacing(18);
            layoutAlbums->setContentsMargins(6,10,6,10);
            layoutAlbums->setAlignment(Qt::AlignLeft);

            for(auto it=albums.begin();it!=albums.end();++it)
            {

                const QVector<Cancion>& albumCanciones=it.value();
                if(albumCanciones.size()<8) continue;

                QFrame*item=new QFrame();
                item->setFixedWidth(150);
                item->setStyleSheet(
                    "QFrame { background:#1a1a1a; border:1px solid #2b2b2b; border-radius:12px; }"
                    "QFrame:hover { background:#202020; border-color:#1DB954; }"
                    );
                QVBoxLayout*vb=new QVBoxLayout(item);
                vb->setSpacing(8);
                vb->setAlignment(Qt::AlignCenter);

                QPushButton*btn=new QPushButton();
                btn->setFixedSize(140,140);
                btn->setCursor(Qt::PointingHandCursor);
                btn->setStyleSheet(
                    "QPushButton{ border:none; background:#222; border-radius:10px; }"
                    "QPushButton:hover{ border:2px solid #1DB954; }"
                    );
                QPixmap pix(albumCanciones.first().getRutaImagen());
                if(!pix.isNull())
                {

                    QPixmap scaled =pix.scaled(btn->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    btn->setIcon(scaled);
                    btn->setIconSize(btn->size());

                }else{

                    btn->setText("Sin imagen");
                    btn->setStyleSheet(btn->styleSheet()+" QPushButton{ color:white; background:#800; }");

                }

                QLabel*lbl=new QLabel("Album de "+albumCanciones.first().getNombreArtista());
                lbl->setStyleSheet("color:#fff; font-weight:700;");
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setWordWrap(true);

                vb->addWidget(btn);
                vb->addWidget(lbl);
                layoutAlbums->addWidget(item);

                // (opcional)
                // connect(btn, &QPushButton::clicked, this, [=](){
                //     auto* r = new ReproductorMusica(albumCanciones, usuario, nullptr, false, "");
                //     r->show();
                //     this->hide();
                // });

            }

            scrollAlbums->setWidget(contenedorAlbums);
            v->addWidget(scrollAlbums);
        }

        root->addWidget(cardAlbums);
    }

    // ---- cierra el scroll ----
    scroll->setWidget(contenido);
    layoutPrincipal->addWidget(scroll);
}

// ---------------------------------------------------------
// Helpers
// ---------------------------------------------------------

QWidget* VisitaPerfilArtista::crearFilaSingles(const QVector<Cancion>& canciones,const QString& tituloFila)
{

    QWidget* seccion=new QWidget();
    seccion->setObjectName("SectionCard");
    seccion->setStyleSheet(
        "#SectionCard { background-color:#151515; border:1px solid #2a2a2a; border-radius:16px; }"
        "#SectionCard QLabel[role='title']{ font-size:20px; font-weight:700; color:#fff; }"
        /* Scrollbar horizontal dentro de la tarjeta */
        "#SectionCard QScrollBar:horizontal{ height:10px; background:#0f0f0f; border:none; }"
        "#SectionCard QScrollBar::handle:horizontal{ background:#1DB954; border-radius:5px; }"
        "#SectionCard QScrollBar::add-line:horizontal,"
        "#SectionCard QScrollBar::sub-line:horizontal{ width:0px; background:transparent; }"
    );

    QVBoxLayout* lay =new QVBoxLayout(seccion);
    lay->setContentsMargins(14,12,14,12);
    lay->setSpacing(8);

    QLabel* titulo= new QLabel(tituloFila);
    titulo->setProperty("role","title");
    lay->addWidget(titulo);

    if(canciones.isEmpty())
    {

        QLabel*vacio=new QLabel("Este artista aun no tiene singles");
        vacio->setStyleSheet("color:#a7a7a7;");
        vacio->setAlignment(Qt::AlignLeft);
        lay->addWidget(vacio);
        return seccion;

    }

    // Scroll horizontal
    QScrollArea* scrollH = new QScrollArea();
    scrollH->setWidgetResizable(true);
    scrollH->setFixedHeight(250);
    scrollH->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollH->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollH->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget*contenedor=new QWidget();
    QHBoxLayout*fila=new QHBoxLayout(contenedor);
    fila->setSpacing(18);
    fila->setContentsMargins(6,10,6,10);
    fila->setAlignment(Qt::AlignLeft);

    for(const Cancion&c:canciones)
    {

        QFrame*card=new QFrame();
        card->setFixedWidth(130);
        card->setFrameShape(QFrame::Box);
        card->setStyleSheet(
            "QFrame { background:#1a1a1a; border:1px solid #2b2b2b; border-radius:12px; }"
            "QFrame:hover { background:#202020; border-color:#1DB954; }"
            );

        QVBoxLayout* vb =new QVBoxLayout(card);
        vb->setSpacing(8);
        vb->setAlignment(Qt::AlignCenter);

        QPushButton* btn= new QPushButton();
        btn->setFixedSize(140,140);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton{ border:none; background:#222; border-radius:10px; }"
            "QPushButton:hover{ border:2px solid #1DB954; }"
            );


        QPixmap pix(c.getRutaImagen());
        if(!pix.isNull())
        {

            QPixmap scaled=pix.scaled(btn->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            btn->setIcon(scaled);
            btn->setIconSize(btn->size());

        }else{

            btn->setText("Sin imagen");
            btn->setStyleSheet("color:white; background-color:#800; border-radius:10px;");

        }

        QLabel* lblTitulo =new QLabel(c.getTitulo());
        lblTitulo->setStyleSheet("color:#fff; font-weight:700;");
        lblTitulo->setAlignment(Qt::AlignCenter);
        lblTitulo->setWordWrap(true);

        QLabel*lblArt = new QLabel(c.getNombreArtista());
        lblArt->setStyleSheet("color:#a7a7a7; font-size:12px;");
        lblArt->setAlignment(Qt::AlignCenter);
        lblArt->setWordWrap(true);

        vb->addWidget(btn);
        vb->addWidget(lblTitulo);
        vb->addWidget(lblArt);

        fila->addWidget(card);

        // connect(btn, &QPushButton::clicked, this, [=]() {
        //     auto* r = new ReproductorMusica(QVector<Cancion>{c}, usuario, nullptr, false, "");
        //     r->show();
        //     this->hide();
        // });
    }

    scrollH->setWidget(contenedor);
    lay->addWidget(scrollH);
    return seccion;
}

QPixmap VisitaPerfilArtista::circularPixmap(const QPixmap& src, const QSize& size) const
{

    QPixmap scaled=src.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap out(size);
    out.fill(Qt::transparent);
    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addEllipse(0,0,size.width(),size.height());
    p.setClipPath(path);
    p.drawPixmap(0,0,scaled);
    p.end();
    return out;

}

void VisitaPerfilArtista::cargarCancionesDelArtista(QVector<Cancion>& singles,QMap<QString, QVector<Cancion>>& eps,QMap<QString, QVector<Cancion>>& albums)
{
    GestorCanciones gestor;
    const QVector<Cancion>todas=gestor.leerCanciones();

    //Filtrr por artista y activa; agrupa por tipo
    for(const Cancion& c:todas)
    {

        if(c.getNombreArtista()!=artista.getNombreArtistico()||!c.estaActiva())
            continue;

        switch(c.getTipo())
        {

        case Tipo::Single:

            singles.append(c);
            break;

        case Tipo::EP:

            eps[artista.getNombreArtistico()].append(c);
            break;

        case Tipo::Album:

            albums[artista.getNombreArtistico()].append(c);
            break;

        default:
            break;
        }
    }
}

void VisitaPerfilArtista::VolverHome()
{

    auto*h=new Home(usuario,nullptr);
    h->show();
    this->hide();

}
