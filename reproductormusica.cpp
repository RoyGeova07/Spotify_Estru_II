#include"reproductormusica.h"
#include<QPixmap>
#include<QStyle>
#include<QDebug>
#include<QFile>
#include<QEventLoop>
#include<QMessageBox>
#include"tipos.h"
#include<QDialog>
#include<QDialogButtonBox>
#include<QComboBox>
#include<QCheckBox>
#include<QFile>
#include<QSaveFile>
#include<QDataStream>
#include<QDir>
#include<QCryptographicHash>
#include<QTextStream>
#include<QDateTime>

ReproductorMusica::ReproductorMusica(const QVector<Cancion>& canciones, const Usuario& usuario, QWidget *parent,bool modoPlaylist,const QString&pathPlaylist): QWidget(parent), listaCanciones(canciones), usuarioActivo(usuario), indiceActual(0),esDesdePlaylist(modoPlaylist),rutaPlaylistDat(pathPlaylist)
{

    setWindowTitle("Reproductor Musical");
    setFixedSize(950,700);
    setStyleSheet("background-color: #121212; color: white;");

    control=new ControlReproduccion(this);
    control->setListaCanciones(listaCanciones);

    auto*player=control->getReproductor();

    //1. actualizar posicion/duracion en variables
    connect(player,&QMediaPlayer::positionChanged,this,[this](qint64 p)
    {

        posMsActual=p;

    });
    connect(player,&QMediaPlayer::durationChanged,this,[this](qint64 p)
    {

        durMsActual=p;

    });

    // 2)Cuando termina el medio, guardamos el evento
    connect(player,&QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus s)
    {

        if(s==QMediaPlayer::EndOfMedia)
        {

            flushEventoActual();

        }

    });

    connect(control, &ControlReproduccion::indiceActualizado, this, [this](int nuevoIdx)
    {

        if(indiceActual>=0)flushEventoActual(); // guarda el que estaba sonando
        indiceActual=nuevoIdx;
        posMsActual=0;
        durMsActual=control->getReproductor()->duration(); // puede ser 0 justo al cambiar; se actualizara luego

    });

    //AQUI SE ACTUALIZA LA BARRA DE PROGRESO Y DURACION DEL REPRODUCTOR DE CONTROL
    connect(control->getReproductor(),&QMediaPlayer::positionChanged,this,&ReproductorMusica::actualizarPosicion);
    connect(control->getReproductor(),&QMediaPlayer::durationChanged,this,&ReproductorMusica::actualizarDuracion);

    this->esDesdePlaylist=modoPlaylist;
    this->rutaPlaylistDat=pathPlaylist;

    configurarInterfaz();

    if(!listaCanciones.isEmpty())
    {

        int idxIni=control->getIndiceActual();
        if(idxIni<0||idxIni>=listaCanciones.size())idxIni=0;

        //selecciona en la lista (dispara currentRowChanged para labels)
        listaWidget->setCurrentRow(idxIni);

        //y ademas, fuerza la sincronizacion del estado del boton
        actualizarUIRating(listaCanciones[idxIni]);

    }

    //AL CAMBIAR DE CANCION
    connect(control,&ControlReproduccion::indiceActualizado,this,[=](int indice)
    {

       listaWidget->setCurrentRow(indice);
        const Cancion&c=listaCanciones[indice];

        lblTitulo->setText("Titulo: "+c.getTitulo());
        lblArtista->setText("Artista: "+c.getNombreArtista());
        lblTipo->setText(tipoToString(c.getTipo()));
        lblReproducciones->setText(QString::number(c.getReproducciones()) + " Reproducciones");
        lblDescripcion->setText("Descripcion:"+c.getDescripcion());

        if (QFile::exists(c.getRutaImagen())) {
            lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(250,250,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        } else {
            lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(250,250,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        }

        //VER SI ESTO ESTA BIEN COLOCADO
        resetEscuchaYRating();
        actualizarUIRating(listaCanciones[indice]);
    });

    //CONNECT PARA REPRODUCIR AL HACER CLICK
    connect(listaWidget,&QListWidget::itemClicked,this,[=](QListWidgetItem*item)
    {

        int index=listaWidget->row(item);
        control->reproducir(index);
        btnPlayPause->setText("⏸️");

    });

    //BOTON DE PLAY Y PAUSA
    connect(btnPlayPause,&QPushButton::clicked,this,[=]()
    {

        QMediaPlayer*rep=control->getReproductor();
        if(rep->playbackState()==QMediaPlayer::PlayingState)
        {

            rep->pause();
            btnPlayPause->setText("▶️");

        }else if(rep->playbackState()==QMediaPlayer::PausedState){

            control->play();
            btnPlayPause->setText("⏸️");

        }else{// StoppedState u otra

            int idx=listaWidget->currentRow();
            if(idx<0)idx=control->getIndiceActual();
            if(idx<0)idx=0;
            control->reproducir(idx);
            btnPlayPause->setText("⏸️");

        }

    });

    //BOTON SIGUIENTE
    connect(btnSiguiente,&QPushButton::clicked,this,[=]()
    {

       control->siguiente();

    });

    //BOTON ANTERIOR
    connect(btnAnterior,&QPushButton::clicked,this,[=]()
    {

        control->anterior();

    });

    bool repetirActivo=false;
    bool aleatorioActivo=false;

    //BOTON REPETIR
    connect(btnRepetir, &QPushButton::clicked, this,[this, &repetirActivo, &aleatorioActivo]()
    {

        repetirActivo=!repetirActivo;

        if(repetirActivo)
        {

            aleatorioActivo=false;
            control->activarAleatorio(false);
            this->btnAleatorio->setStyleSheet("");

        }

        control->activarRepetir(repetirActivo);
        this->btnRepetir->setStyleSheet(repetirActivo ? "background-color: #1DB954;" : "");

    });

    //BOTON ALEATORIO
    // BOTÓN ALEATORIO
    connect(btnAleatorio, &QPushButton::clicked, this,[this, &repetirActivo, &aleatorioActivo]()
    {

        aleatorioActivo=!aleatorioActivo;

        if(aleatorioActivo)
        {

            repetirActivo=false;
            control->activarRepetir(false);
            this->btnRepetir->setStyleSheet("");

        }

        control->activarAleatorio(aleatorioActivo);
        this->btnAleatorio->setStyleSheet(aleatorioActivo ? "background-color: #1DB954" : "");

    });
}

void ReproductorMusica::configurarInterfaz()
{
    QVBoxLayout* mainLayout =new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Usuario
    lblUsuario= new QLabel("Escuchando como: " + usuarioActivo.getNombreUsuario());
    lblUsuario->setStyleSheet("font-size: 14px; color: gray;");
    mainLayout->addWidget(lblUsuario, 0, Qt::AlignLeft);

    //Info: caratula + titulo/artista
    QHBoxLayout* infoLayout = new QHBoxLayout();

    lblCaratula = new QLabel();
    lblCaratula->setFixedSize(250, 250);
    lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    lblCaratula->setStyleSheet("border-radius: 10px;");
    infoLayout->addWidget(lblCaratula);

    QVBoxLayout* textoLayout=new QVBoxLayout();

    lblTipo=new QLabel("Tipo");
    lblTipo->setStyleSheet("font-size: 14px; color: #b3b3b3;");

    lblTitulo=new QLabel("Título de la canción");
    lblTitulo->setStyleSheet("font-size: 48px; font-weight: bold; color: white; margin-bottom: 10px;");

    lblArtista=new QLabel("Artista");
    lblArtista->setStyleSheet("font-size: 20px; color: #b3b3b3;");

    QHBoxLayout*rateLayout=new QHBoxLayout();

    lblRatingPromedio=new QLabel("⭐ Sin calificaciones");
    lblRatingPromedio->setStyleSheet("font-size:14px; color:#b3b3b3;");

    btnCalificar =new QPushButton("⭐ Calificar");
    btnCalificar->setCursor(Qt::PointingHandCursor);
    btnCalificar->setEnabled(false); // se habilita al escuchar ≥30s
    btnCalificar->setToolTip("Disponible tras escuchar 30 segundos");
    btnCalificar->setStyleSheet(
        "QPushButton { background-color:#ff9800; color:white; border:none;"
        "padding:6px 12px; border-radius:14px; font-weight:600; }"
        "QPushButton:disabled { opacity:0.95; }"
        );//NARANJA POR DEFECTO

    rateLayout->addWidget(lblRatingPromedio);
    rateLayout->addSpacing(8);
    rateLayout->addWidget(btnCalificar);
    rateLayout->addStretch();
    textoLayout->addLayout(rateLayout);

    //conexion del boton
    connect(btnCalificar,&QPushButton::clicked,this,&ReproductorMusica::calificarActual);

    lblReproducciones=new QLabel("0 Reproducciones");
    lblReproducciones->setStyleSheet("font-size: 16px; color: #b3b3b3;");

    lblDescripcion=new QLabel("Descripcion:");
    lblDescripcion->setStyleSheet("font-size: 14px; color: #b3b3b3;");
    lblDescripcion->setWordWrap(true);
    lblDescripcion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    lblGenero=new QLabel("Genero: ");
    lblGenero->setStyleSheet("font-size: 14px; color: #b3b3b3;");
    lblGenero->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);


    lblFechaCarga=new QLabel("Fecha Carga: ");
    lblFechaCarga->setStyleSheet("font-size: 14px; color: #b3b3b3;");
    lblFechaCarga->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    textoLayout->addWidget(lblTipo);
    textoLayout->addWidget(lblTitulo);
    textoLayout->addWidget(lblArtista);
    //textoLayout->addWidget(lblReproducciones);
    textoLayout->addWidget(lblDescripcion);
    textoLayout->addWidget(lblGenero);
    textoLayout->addWidget(lblFechaCarga);
    textoLayout->addStretch();

    infoLayout->addLayout(textoLayout);
    mainLayout->addLayout(infoLayout);

    // Barra de progreso
    barraProgreso = new SliderClickable(Qt::Horizontal);
    barraProgreso->setRange(0, 0);
    barraProgreso->setStyleSheet("QSlider::groove:horizontal { background: #333; height: 4px; }"
                                 "QSlider::handle:horizontal { background: #ff3333; width: 12px; }");

    //Timer para medir tiempo REAL escuchado (no por posicion)
    tEscucha=new QTimer(this);
    tEscucha->setInterval(500);
    connect(tEscucha,&QTimer::timeout,this,[this](){

        auto*rep=control->getReproductor();
        if(!rep)return;
        if(rep->playbackState()==QMediaPlayer::PlayingState&&indiceActual>=0)
        {

            escuchadoMsAcum+=tEscucha->interval();
            if(!habilitaRate&&escuchadoMsAcum>=30'000)
            {

                habilitaRate=true;
                //si no ha calificado esta cancion, habilitar boton
                const Cancion&c=listaCanciones[indiceActual];
                GestorCalificaciones gc;
                quint8 prev=0;
                if(!gc.yaCalifico(static_cast<quint32>(usuarioActivo.getId()),static_cast<quint32>(c.getId()),&prev))
                {

                    btnCalificar->setEnabled(true);
                    btnCalificar->setToolTip("Calificar esta cancion (1-5)");
                    btnCalificar->setStyleSheet(
                        "QPushButton { background-color:#1DB954; color:white; border:none;"
                        "padding:6px 12px; border-radius:14px; font-weight:600; }"
                        );//VERDE CUANDO YA SE PUEDE CALIFICAR

                }

            }

        }

    });
    //arrancar/detener timer segun el estado del player
    auto*player=control->getReproductor();
    connect(player,&QMediaPlayer::playbackStateChanged,this,[this](QMediaPlayer::PlaybackState s){

        if(s==QMediaPlayer::PlayingState) tEscucha->start(); else tEscucha->stop();

    });


    connect(barraProgreso,&QSlider::sliderReleased,this,[=]()
    {

        qint64 nuevaPosicion=barraProgreso->value();
        control->getReproductor()->setPosition(nuevaPosicion);

    });

    //MIENTRAS MUEVE EL MOUSE, SE ACTUALIZA TAMBIEN EL TIEMPO
    connect(barraProgreso,&QSlider::sliderMoved,this,[=](int value){

        lblTiempoActual->setText(formatoTiempo(value));

    });

    lblTiempoActual=new QLabel("0:00");
    lblDuracionTotal=new QLabel("0:00");

    QHBoxLayout* tiempoLayout = new QHBoxLayout();
    tiempoLayout->addWidget(lblTiempoActual);
    tiempoLayout->addWidget(barraProgreso);
    tiempoLayout->addWidget(lblDuracionTotal);
    mainLayout->addLayout(tiempoLayout);

    // Controles
    QHBoxLayout*controlesLayout =new QHBoxLayout();

    btnAnterior=new QPushButton("⏮️");
    btnAnterior->setToolTip("Pasar a la musica anterior");

    btnPlayPause=new QPushButton("▶️");
    btnPlayPause->setToolTip("Play Musica");

    btnSiguiente=new QPushButton("⏭️");
    btnSiguiente->setToolTip("Pasar a la musica siguiente");

    btnAleatorio= new QPushButton("🔀");
    btnAleatorio->setToolTip("Reproduccion Aleatoria");

    btnRepetir =new QPushButton("🔁");
    btnRepetir->setToolTip("Repetir Cancion infinitamente");

    btnCerrar =new QPushButton("<-- Volver");
    btnCerrar->setToolTip("Volver al home");

    QList<QPushButton*>botones={btnAleatorio, btnAnterior, btnPlayPause, btnSiguiente, btnRepetir,btnCerrar};
    for(auto boton:botones)
    {

        boton->setCursor(Qt::PointingHandCursor);
        controlesLayout->addWidget(boton);

    }

    mainLayout->addLayout(controlesLayout);

    // ENCABEZADO TIPO SPOTIFY
    QWidget*encabezadoWidget=new QWidget();
    encabezadoWidget->setFixedHeight(30);
    encabezadoWidget->setStyleSheet("background-color: #121212;");
    QHBoxLayout* encabezadoLayout = new QHBoxLayout(encabezadoWidget);
    encabezadoLayout->setContentsMargins(10, 0, 10, 0);

    QLabel*lblNum=new QLabel("#");
    lblNum->setFixedWidth(30);
    lblNum->setStyleSheet("color: #b3b3b3; font-size: 14px;");

    QLabel*lblTituloEnc=new QLabel("Título");
    lblTituloEnc->setStyleSheet("color: #b3b3b3; font-size: 14px;");
    lblTituloEnc->setMinimumWidth(300);

    QLabel*lblReproEnc= new QLabel("Reproducciones");
    lblReproEnc->setStyleSheet("color: #b3b3b3; font-size: 14px;");
    lblReproEnc->setAlignment(Qt::AlignRight);
    lblReproEnc->setFixedWidth(130);

    const int W_DUR=80;     // mismo ancho que la duracion en cada fila
    const int W_BTN=120;    // mismo ancho que el boton (Descargar / Eliminar)
    const int GAP=10;     // separacion entre duracion y boton

    // Contenedor derecho: [⏱ (W_DUR)] [GAP] [placeholder boton (W_BTN)]
    QWidget* rightHeader = new QWidget;
    rightHeader->setFixedWidth(W_DUR+GAP+W_BTN);
    QHBoxLayout* rightLay = new QHBoxLayout(rightHeader);
    rightLay->setContentsMargins(0,0,0,0);
    rightLay->setSpacing(GAP);

    QLabel* lblDuracionEnc = new QLabel("⏱");
    lblDuracionEnc->setFixedWidth(W_DUR);
    lblDuracionEnc->setAlignment(Qt::AlignRight);
    lblDuracionEnc->setStyleSheet("color: #b3b3b3; font-size: 14px;");

    QLabel*lblDownloadEnc=new QLabel("📥");
    lblDownloadEnc->setFixedWidth(W_BTN);
    lblDownloadEnc->setAlignment(Qt::AlignHCenter);
    lblDownloadEnc->setStyleSheet("color: #b3b3b3; font-size: 14px;");


    QWidget* actionsPh = new QWidget;
    actionsPh->setFixedWidth(W_BTN);

    rightLay->addWidget(lblDuracionEnc);
    rightLay->addWidget(lblDownloadEnc);
    rightLay->addWidget(actionsPh);

    encabezadoLayout->addWidget(lblNum);
    encabezadoLayout->addWidget(lblTituloEnc, 1);
    //encabezadoLayout->addWidget(lblReproEnc);
    encabezadoLayout->addWidget(rightHeader);
    mainLayout->addWidget(encabezadoWidget);

    //Lista de canciones (ahora al final)
    if(listaCanciones.size()>0)
    {

        listaWidget=new QListWidget();
        listaWidget->setStyleSheet(
            "QListWidget { background-color: #121212; border: none; color: white; }"
            "QListWidget::item { padding: 10px; }"
            "QListWidget::item:selected { background-color: transparent; color: #1DB954; font-weight: bold; }"
            );
        listaWidget->setFixedHeight(250);//TAMANIO DE LA PALETA DE LAS MUSICAS

        //CARD DE LAS MUSICAS
        for(int i=0; i<listaCanciones.size();++i)
        {

            const Cancion&c=listaCanciones[i];
            QWidget*filaWidget=new QWidget();
            filaWidget->setMinimumHeight(40);//PALETA INTERIOR DE LAS MUSICAS
            QHBoxLayout*filaLayout=new QHBoxLayout(filaWidget);
            filaLayout->setContentsMargins(10,5,10,5);

            //Numero
            QLabel*lblNumero = new QLabel(QString::number(i+1));
            lblNumero->setFixedWidth(30);
            lblNumero->setStyleSheet("color: #b3b3b3; font-size: 14px;");
            lblNumero->setObjectName("numLbl");

            //Titulo + Artista
            QVBoxLayout*tituloArtistaLayout=new QVBoxLayout();
            tituloArtistaLayout->setSpacing(2);

            QLabel*lblTituloA=new QLabel(c.getTitulo());
            lblTituloA->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
            lblTituloA->setWordWrap(true);
            lblTituloA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

            QLabel*lblArtistaB=new QLabel(c.getNombreArtista());
            lblArtistaB->setStyleSheet("color: #b3b3b3; font-size: 13px;");
            lblArtistaB->setWordWrap(true);
            lblArtistaB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

            tituloArtistaLayout->addWidget(lblTituloA);
            tituloArtistaLayout->addWidget(lblArtistaB);

            // Reproducciones
            QLabel*lblReproducciones=new QLabel(QString::number(c.getReproducciones()));
            lblReproducciones->setFixedWidth(130);
            lblReproducciones->setAlignment(Qt::AlignRight);
            lblReproducciones->setStyleSheet("color: #b3b3b3; font-size: 14px;");

            //Duracion
            QString duracionStr;
            if(c.getDuracion().toInt()==0&&QFile::exists(c.getRutaAudio()))
            {

                QMediaPlayer*tempPlayer=new QMediaPlayer;
                tempPlayer->setSource(QUrl::fromLocalFile(c.getRutaAudio()));
                QEventLoop loop;
                QObject::connect(tempPlayer, &QMediaPlayer::durationChanged, &loop, &QEventLoop::quit);
                loop.exec();
                duracionStr=formatoTiempo(tempPlayer->duration());
                delete tempPlayer;

            }else{

                duracionStr=formatoTiempo(c.getDuracion().toInt());

            }

            QLabel* lblDuracion = new QLabel(duracionStr);
            lblDuracion->setFixedWidth(80);
            lblDuracion->setAlignment(Qt::AlignRight);
            lblDuracion->setStyleSheet("color: #b3b3b3; font-size: 14px;");

            QPushButton*btnPrimario=nullptr;

            QListWidgetItem*item=new QListWidgetItem(listaWidget);//DECLARAR AQUI SIN BUGUEAR LAS CARDS DE LAS MUSICAS

            if(!esDesdePlaylist)
            {

                btnPrimario=new QPushButton("➕ Descargar");
                btnPrimario->setCursor(Qt::PointingHandCursor);
                btnPrimario->setFixedHeight(28);
                btnPrimario->setStyleSheet(
                    "QPushButton { background-color: #1DB954; color: white; border: none; "
                    "padding: 6px 12px; border-radius: 14px; font-weight: 600; }"
                    "QPushButton:hover { background-color: #1ed760; }"
                    );

                connect(btnPrimario,&QPushButton::clicked,this,[=](){

                    guardarEnPlaylist(i);

                });

            }else{

                // MODO playlist: botón "Eliminar"
                btnPrimario=new QPushButton("🗑 Eliminar");
                btnPrimario->setCursor(Qt::PointingHandCursor);
                btnPrimario->setFixedHeight(28);
                btnPrimario->setStyleSheet(
                    "QPushButton { background-color: #bb2d3b; color: white; border: none; "
                    "padding: 6px 12px; border-radius: 14px; font-weight: 600; }"
                    "QPushButton:hover { background-color: #d24554; }"
                    );
                connect(btnPrimario, &QPushButton::clicked, this, [=]()
                {

                    int idxActual=listaWidget->row(item);
                    eliminarDePlaylist(idxActual);

                });

            }

            // Agregar al layout
            filaLayout->addWidget(lblNumero);
            filaLayout->addLayout(tituloArtistaLayout, 1);


           // filaLayout->addWidget(lblReproducciones);
            filaLayout->addWidget(lblDuracion);
            //Empujar el boton al extremo derecho
            filaLayout->addSpacing(10);
            filaLayout->addStretch();
            filaLayout->addWidget(btnPrimario);

            item->setSizeHint(filaWidget->sizeHint());
            listaWidget->addItem(item);
            listaWidget->setItemWidget(item, filaWidget);

        }

        mainLayout->addWidget(listaWidget);

        //AL DARLE CLICK A LAS MUSICAS
        connect(listaWidget,&QListWidget::currentRowChanged,this,[=](int columna){

            if(columna>=0&&columna<listaCanciones.size())
            {

                const Cancion&c=listaCanciones[columna];

                lblTipo->setText(tipoToString(c.getTipo()));
                lblTitulo->setText("Titulo: "+c.getTitulo());
                lblArtista->setText("Artista: "+c.getNombreArtista());
                lblDescripcion->setText("Descripcion:"+c.getDescripcion());
                lblGenero->setText("Genero: "+generoToString(c.getGenero()));
                lblFechaCarga->setText("Fecha Carga: "+c.getFechaCarga().toString("dd/MM/yyyy"));

                //VALIDACION SI TIENE CARATULA VALIDA
                if(QFile::exists(c.getRutaImagen()))
                {

                    lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(250,250,Qt::KeepAspectRatio,Qt::SmoothTransformation));

                }else{

                    lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));

                }

                //VER SI ESTO ESTA BIEN COLOCADO
                resetEscuchaYRating();
                actualizarUIRating(c);
            }

        });

    }
    connect(btnCerrar, &QPushButton::clicked, this, &ReproductorMusica::cerrarReproductor);

}

void ReproductorMusica::cerrarReproductor()
{

    QMediaPlayer*rep=control ? control->getReproductor() : nullptr;

    if(rep&&rep->playbackState()==QMediaPlayer::PlayingState)
    {

        auto r= QMessageBox::question(this, "Detener reproducción","¿Deseas cerrar el reproductor? La música dejará de escucharse.",QMessageBox::Yes |QMessageBox::No);
        if(r!=QMessageBox::Yes)return;

        //guardar lo escuchado antes de detener (poara no perder la posicion)
        flushEventoActual();

        control->detener();

    }else{

        flushEventoActual();

    }

    navegandoAHome=true;
    const bool cerro=QWidget::close();
    navegandoAHome=false;

    if(cerro)
    {

        Home*h=new Home(usuarioActivo,nullptr);
        h->show();

    }


}

void ReproductorMusica::actualizarPosicion(qint64 position)
{

    posMsActual=position;
    barraProgreso->setValue(position);
    lblTiempoActual->setText(formatoTiempo(position));

}

void ReproductorMusica::actualizarDuracion(qint64 duration)
{

    durMsActual=duration;
    barraProgreso->setMaximum(duration);
    lblDuracionTotal->setText(formatoTiempo(duration));

}

QString ReproductorMusica::formatoTiempo(qint64 ms)
{

    int segundos=ms/1000;
    int minutos=segundos/60;
    segundos%=60;
    return QString::number(minutos)+":"+QString("%1").arg(segundos,2,10,QChar('0'));

}

//closeEvent se ejecuta cuando se intenta cerrar la ventana.
void ReproductorMusica::closeEvent(QCloseEvent *event)
{

    QMediaPlayer*rep=control?control->getReproductor() : nullptr;

    //Si la musica esta sonando, se muestra una advertencia.
    if(!navegandoAHome)
    {

        if(rep&&rep->playbackState()==QMediaPlayer::PlayingState)
        {

            auto respuesta=QMessageBox::question(this,"Detener reproduccion","¿Deseas cerrar el reproductor? La musica dejara de escucharse.",QMessageBox::Yes|QMessageBox::No);
            if(respuesta!=QMessageBox::Yes)
            {

                event->ignore();  //no cierra la ventana
                return;

            }

            //guardar antes de detener
            flushEventoActual();

            control->detener();
        }else{

            // Pausa/Stop: guardar por si habia progreso
            flushEventoActual();

            control->detener(); //por si habia algo cargado en pausa

        }

    }else{

        //Navegacion programatica (volver): igual nos aseguramos
        flushEventoActual();
        if(control)control->detener();

    }
    event->accept(); // cerrar la ventana

}

/*

    Antes de esta logica, si se pulsaba ➕ Agregar varias veces para la misma cancion y la misma playlist,
    el programa volveria a guardarla dentro del .dat sin control, porque no habia manera rapida de saber
    si ya estaba dentro.

    Para evitar eso sin tener que leer todo el .dat (lo cual es mas lento y complejo), se creo un archivo paralelo
    .idx que almacena identificadores unicos (keys) de cada cancion que ya fue agregada

    en resumen el .idx es como la lista de nombres de las personas que ya estan dentro de una fiesta.

    playlist1.dat = la fiesta en si, con todos los datos completos de cada invitado (cancion).

    playlist1.dat.idx = la lista en la puerta con un identificador unico de cada invitado para que no entren dos veces.

    Ejemplo:

    Llega Juan (canción A) -> No esta en la lista (.idx) -> entra a la fiesta (.dat) y se apunta su nombre en la lista (.idx).

    Llega otra vez Juan (cancion A) -> El portero mira la lista (.idx) -> “Ya estas dentro, no entras otra vez”.

    Asi evitamos que se guarde dos veces la misma cancion sin tener que revisar toda la fiesta (todo el .dat).


    la llave que se genera a partir de la informacion unica de la cancion, para que siempre salga igual para la misma cancion
    y distinto para canciones diferentes.

    Si la cancion tiene ruta de audio valida:
    Usamos la ruta completa del archivo (C:/Musica/Album/Cancion.mp3).

    Convertimos ese texto base a un hash SHA-1 (función de huella digital).
    Esto devuelve un código hexadecimal de 40 caracteres, algo como:

    e242ed3bffccdf271b7fbaf34ed72d089537b42

    Guardamos ese hash en el .idx como la “huella” de la cancion.

    La proxima vez que intentes guardar esa cancion, comparamos su huella con las que ya estan en el .idx.
    Si coincide, significa que ya existe y no se agrega de nuevo.

*/

void ReproductorMusica::guardarEnPlaylist(int indexCancion)
{

    if(indexCancion<0||indexCancion>=listaCanciones.size())
    {

        QMessageBox::warning(this, "Playlist", "Indice de cancion invalido.");
        return;

    }

    const Cancion& cSel=listaCanciones[indexCancion];

    // 1) Localizar carpeta del usuario y sus .dat
    const QString baseDir="Publico";
    const QString carpetaUsuario=baseDir+"/Usuario_"+usuarioActivo.getNombreUsuario();
    QDir dir(carpetaUsuario);

    if(!dir.exists())
    {

        QMessageBox::warning(this, "Playlist", "Aun no tienes carpeta de usuario.");
        return;

    }
    const QStringList archivosDat=dir.entryList(QStringList()<<"*.dat", QDir::Files, QDir::Name);
    if(archivosDat.isEmpty())
    {

        QMessageBox::information(this, "Playlist", "No hay playlists. Crea una primero en Home.");
        return;

    }
    // 2) Dialogo con combo de playlists
    QDialog dlg(this);
    dlg.setWindowTitle("Agregar a playlist");
    QVBoxLayout*v=new QVBoxLayout(&dlg);

    QLabel*info=new QLabel(QString("Canción: <b>%1</b><br/>Artista: %2").arg(cSel.getTitulo(), cSel.getNombreArtista()));
    info->setTextFormat(Qt::RichText);
    v->addWidget(info);

    QHBoxLayout*fila=new QHBoxLayout();
    fila->addWidget(new QLabel("Playlist:"));
    QComboBox*cbo=new QComboBox();
    for(const QString& f:archivosDat)
    {

        const QString nombre=QFileInfo(f).completeBaseName();
        cbo->addItem(nombre,f); // data = nombreArchivo.dat

    }
    fila->addWidget(cbo,1);
    v->addLayout(fila);


    // Mensaje de validacion en vivo
    QLabel* lblValid = new QLabel;
    lblValid->setStyleSheet("color:#b3b3b3;");
    v->addWidget(lblValid);

    QDialogButtonBox* box =new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    v->addWidget(box);

    auto actualizarEstado=[&]()
    {

        const QString nombreArchivo=cbo->currentData().toString();
        const QString pathDat=carpetaUsuario+"/"+nombreArchivo;

        if(cancionYaEnPlaylist(pathDat, cSel))
        {

            lblValid->setText("⚠️ Esta cancion ya esta en la playlist seleccionada.");
            box->button(QDialogButtonBox::Ok)->setEnabled(false);

        }else{

            lblValid->setText("✔️ Lista para agregar.");
            box->button(QDialogButtonBox::Ok)->setEnabled(true);

        }

    };

    QObject::connect(cbo, &QComboBox::currentIndexChanged, &dlg, [&](int){ actualizarEstado(); });
    QObject::connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    actualizarEstado();
    if(dlg.exec()!=QDialog::Accepted)return;

    // 3) Guardar realmente en binario (.dat) y actualizar indice (.idx)
    const QString nombreArchivo=cbo->currentData().toString();
    const QString pathDat=carpetaUsuario+"/" + nombreArchivo;
    const QString idxPath=pathDat+".idx";

    // Recheck por si cambio la selección mientras estaba abierto el dialogo
    if(cancionYaEnPlaylist(pathDat, cSel))
    {

        QMessageBox::warning(this, "Playlist", "La canción ya está en esa playlist.");
        return;

    }


    QFile f(pathDat);
    if(!f.open(QIODevice::Append))
    {

        QMessageBox::critical(this, "Playlist", "No se pudo abrir el archivo de la playlist para escribir.");
        return;

    }
    QDataStream out(&f);
    out.setVersion(QDataStream::Qt_5_15);

    // Encabezado por registro + payload
    const quint32 kMagic = 0x534F4E47; // 'SONG'
    const quint16 kVersion = 2;//v2 con id

    out <<kMagic<<kVersion;
    out<<static_cast<quint32>(qMax(0,cSel.getId()));//id primero

    //Serializar campos visibles del reproductor
    out<<cSel.getTitulo();
    out<<cSel.getNombreArtista();
    out<<static_cast<quint16>(cSel.getTipo());
    out<<cSel.getDescripcion();
    out<<static_cast<quint16>(cSel.getGenero());
    out<<cSel.getFechaCarga();
    out<<cSel.getDuracion();
    out<<cSel.getRutaAudio();
    out<<cSel.getRutaImagen();
    out<<static_cast<qint32>(cSel.getReproducciones());
    out<<static_cast<bool>(cSel.estaActiva());

    f.close();

    // 4) Actualizar/crear indice de duplicados
    QFile idx(idxPath);
    if(idx.open(QIODevice::Append|QIODevice::Text))
    {

        QTextStream ts(&idx);
        ts<<keyCancion(cSel)<<"\n";
        idx.close();

    }
    // 5) Confirmacion
    QMessageBox::information(this, "Playlist",QString("Se agrego \"%1\" a la playlist \"%2\".").arg(cSel.getTitulo(), QFileInfo(nombreArchivo).completeBaseName()));

}

//Genera una clave estable para detectar duplicados
QString ReproductorMusica::keyCancion(const Cancion &c) const
{

    QString base;
    if(c.getId()>0)
    {

        base="ID: "+QString::number(c.getId());

    }else if(!c.getRutaAudio().isEmpty()){

        base=QFileInfo(c.getRutaAudio()).absoluteFilePath();

    }else{

        base=c.getTitulo()+"|"+c.getNombreArtista()+"|"+QString::number(c.getDuracion().toInt());

    }

    QByteArray hash=QCryptographicHash::hash(base.toUtf8(),QCryptographicHash::Sha1);
    return QString::fromLatin1(hash.toHex());// 40 hex chars

}
// Lee el indice <playlist>.idx y verifica si ya existe la cancion
bool ReproductorMusica::cancionYaEnPlaylist(const QString& pathDat, const Cancion& c) const
{

    const QString idxPath=pathDat + ".idx";   //indice liviano por playlist
    QFile f(idxPath);
    if (!f.exists())
        return false; // aun no hay indice: no podemos afirmar duplicado

    if(!f.open(QIODevice::ReadOnly|QIODevice::Text))
        return false; // si no podemos leer, no bloqueamos por ahora

    const QString key=keyCancion(c);
    QTextStream in(&f);
    while(!in.atEnd())
    {

        QString line=in.readLine().trimmed();
        if (line==key)return true;
    }
    return false;
}

void ReproductorMusica::eliminarDePlaylist(int indexFila)
{

    if(!esDesdePlaylist || rutaPlaylistDat.isEmpty())
    {

        QMessageBox::warning(this, "Playlist", "No estas en modo playlist.");
        return;

    }
    if(indexFila<0||indexFila>=listaCanciones.size()) return;

    const Cancion&c=listaCanciones[indexFila];
    const bool disponible=estaEnCatalogoVigente(c);

    const QString titulo=disponible?QStringLiteral("Eliminar de la playlist"):QStringLiteral("Eliminar de tu biblioteca");

    const QString texto=disponible? QStringLiteral("¿Eliminar esta canción de la playlist?"): QStringLiteral("¿Estás seguro de eliminar esta música de tu biblioteca?\n""Si la borras no la podrás volver a descargar debido a que el artista ya ""borró esa canción de su catálogo.\n\n""Presione Sí para eliminar la canción.");

    if(QMessageBox::question(this, titulo, texto,QMessageBox::Yes | QMessageBox::No)!=QMessageBox::Yes)
    {

        return;

    }
    // 1) Leer todo el .dat, saltando el indexFila
    QVector<Cancion> reescribir;
    reescribir.reserve(listaCanciones.size()-1);
    for(int i=0;i<listaCanciones.size();++i)
    {

        if(i!=indexFila)reescribir.append(listaCanciones[i]);

    }

    // 2) Reescribir el .dat desde cero con el mismo formato ('SONG', v1)
    QFile wf(rutaPlaylistDat);
    if(!wf.open(QIODevice::WriteOnly))
    {

        QMessageBox::critical(this, "Playlist", "No se pudo reescribir la playlist.");
        return;

    }
    QDataStream out(&wf);
    out.setVersion(QDataStream::Qt_5_15);
    const quint32 kMagic=0x534F4E47; // 'SONG'
    const quint16 kVersion=2;

    for(const Cancion&cSel:reescribir)
    {

        out<<kMagic<<kVersion;
        out<<static_cast<quint32>(qMax(0,cSel.getId()));
        out<<cSel.getTitulo();
        out<<cSel.getNombreArtista();
        out<<static_cast<quint16>(cSel.getTipo());
        out<<cSel.getDescripcion();
        out<<static_cast<quint16>(cSel.getGenero());
        out<<cSel.getFechaCarga();
        out<<cSel.getDuracion();
        out<<cSel.getRutaAudio();
        out<<cSel.getRutaImagen();
        out<<static_cast<qint32>(cSel.getReproducciones());
        out<<static_cast<bool>(cSel.estaActiva());

    }
    wf.close();

    // 3) Reescribir el .idx con las keys de lo que quedó
    QFile idx(rutaPlaylistDat + ".idx");
    if(idx.open(QIODevice::WriteOnly | QIODevice::Text))
    {

        QTextStream ts(&idx);
        for(const Cancion&c:reescribir)ts<<keyCancion(c)<<"\n";
        idx.close();

    }

    //4) Actualizar datos en memoria
    listaCanciones.removeAt(indexFila);

    //5)Actualizar la UI (quitar la fila y reindexar)
    QListWidgetItem*it=listaWidget->takeItem(indexFila);
    if(it)
    {

        if(QWidget*row=listaWidget->itemWidget(it))
        {

            row->deleteLater();

        }
        delete it;

    }

    //Reindexar los numeros # visibles
    reindexarNumeros(listaWidget);

    // 6) Actualizar la lista en el controlador del reproductor
    control->setListaCanciones(listaCanciones);

    //con esto detecta si la cancion eliminada era la que sonaba
    const bool estabaReproduciendo=(control->getReproductor()->playbackState()==QMediaPlayer::PlayingState);

    const int idxEnUI=listaWidget->currentRow();
    const bool borraLaQueSonaba=(idxEnUI==indexFila);

    //7) Ajustar seleccion / reproduccion actual
    int count=listaWidget->count();
    if(count==0)
    {

        // No quedan canciones: detener y limpiar labels básicos
        control->detener();
        lblTitulo->setText("Sin canciones");
        lblArtista->setText("");
        lblTipo->setText("");
        lblReproducciones->setText("");
        lblDescripcion->setText("");
        lblGenero->setText("");
        lblFechaCarga->setText("");
        lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(250,250,Qt::KeepAspectRatio,Qt::SmoothTransformation));

    }else{

        // Seleccionar una fila valida
        int target=qMin(indexFila,count-1);
        listaWidget->setCurrentRow(target);

        //si borre la que estaba sonando, detengo playBack
        if(borraLaQueSonaba&&estabaReproduciendo)
        {

            control->detener();

        }

    }

    //8) aviso
    QMessageBox::information(this, "Playlist", "Cancion eliminada de la playlist.");

}

bool ReproductorMusica::estaEnCatalogoVigente(const Cancion &c) const
{

    GestorCanciones gestor;
    const QVector<Cancion>todas=gestor.leerCanciones();

    auto norm=[](QString s)
    {

        return s.trimmed().toLower().simplified();

    };

    //Preferir ID si es valido
    if(c.getId()>0)
    {

        for(const Cancion&x:todas)
        {

            if(x.getId()==c.getId())
            {

                return x.estaActiva();   // <- sin QFile::exists

            }

        }
    }

    //Fallback por Título + Artista (para playlists sin id)
    const QString tRef=norm(c.getTitulo());
    const QString aRef=norm(c.getNombreArtista());
    for(const Cancion&x:todas)
    {

        if(norm(x.getTitulo())==tRef&&norm(x.getNombreArtista())==aRef)
        {

            return x.estaActiva();       // <- sin QFile::exists

        }

    }

    return false;

}
/*

Si se escucha 20 s de un tema de 3 min:

Tiempo total: +20 s

Canciones escuchadas: 0 (no llego a 30s)

Si se escucha 35 s y sales:

Tiempo total: +35 s

Canciones escuchadas:+1

*/
void ReproductorMusica::flushEventoActual()
{

    if(indiceActual<0||indiceActual>=listaCanciones.size())return;

    const Cancion&c=listaCanciones[indiceActual];

    quint32 dur=static_cast<quint32>(qMax<qint64>(0,durMsActual));

    // Fallback: usa la duracion de la cancion (en ms) si el player aun no la reporto
    if(dur==0)
    {

        bool ok=false;
        int ms=c.getDuracion().toInt(&ok);// este modelo guarda duracion en MILISEGUNDOS
        if(ok &&ms>0)dur=static_cast<quint32>(ms);

    }

    //Solo tiempo REAL escuchado (no la posicion ni los saltos con el slider)
    quint64 played64=qMax<quint64>(0,escuchadoMsAcum);
    if(dur>0)played64=qMin<qint64>(played64,static_cast<qint64>(dur));
    quint32 played=static_cast<quint32>(played64);


    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const quint32 uid = static_cast<quint32>(usuarioActivo.getId());
    const quint32 sid = static_cast<quint32>(c.getId());

    logger.registrar(uid, sid, played, dur, now);
    //Evitar doble contabilizacion si se vuelve a flushear sin mover reproduccion
    escuchadoMsAcum=0;
    posMsActual=0;
    durMsActual=0;
    /*

    Con esto, cada cambio de pista o final de tema queda append en reproducciones.dat.
    Despues statsUsuario(...) hara los cálculos de:

    totalMs = suma de ms escuchados

    totalEscuchas = cuantas cumplen “escucha valida” (≥ min(30s, 50%)), y

    top = (songId, conteo) ordenado por mas escuchadas.

    */

}

void ReproductorMusica::resetEscuchaYRating()
{

    escuchadoMsAcum=0;
    habilitaRate=false;
    if(btnCalificar)
    {

        btnCalificar->setEnabled(false);
        btnCalificar->setToolTip("Disponible tras escuchar 30 segundos");
        btnCalificar->setStyleSheet(
            "QPushButton { background-color:#ff9800; color:white; border:none;"
            "padding:6px 12px; border-radius:14px; font-weight:600; }"
            "QPushButton:disabled { opacity:0.95; }"
            );//SE PONE EN NARANJA MIENTRAS PASAN LOS 30 SEGUNDOS

    }
    //si la pista actual ya fue calificada, bloquear de un solo
    if(indiceActual>=0&&indiceActual<listaCanciones.size()&&btnCalificar)
    {

        const Cancion&c=listaCanciones[indiceActual];
        GestorCalificaciones gc;
        quint8 prev=0;
        if(gc.yaCalifico(static_cast<quint32>(usuarioActivo.getId()),static_cast<quint32>(c.getId()),&prev))
        {

            btnCalificar->setEnabled(false);
            btnCalificar->setToolTip(QString("Ya calificaste: %1★").arg(prev));
            btnCalificar->setStyleSheet(
                "QPushButton { background-color:#bb2d3b; color:white; border:none;"
                "padding:6px 12px; border-radius:14px; font-weight:600; }"
                );//SE PONE EN ROJO SI YA FUE CALIFICADA

        }

    }

}

void ReproductorMusica::actualizarUIRating(const Cancion &c)
{

    GestorCalificaciones gc;
    double avg=0.0;int cnt=0;
    gc.promedioCancion(static_cast<quint32>(c.getId()),avg,cnt);
    if(cnt>0)
    {

        lblRatingPromedio->setText(QString("⭐ %1 (%2)").arg(QString::number(avg,'f',1)).arg(cnt));

    }else{

        lblRatingPromedio->setText("⭐ Sin calificaciones");

    }
    //si ya califico, bloquea el boton y muestra su voto
    quint8 previo=0;
    if(gc.yaCalifico(static_cast<quint32>(usuarioActivo.getId()),static_cast<quint32>(c.getId()), &previo))
    {

        btnCalificar->setEnabled(false);
        btnCalificar->setToolTip(QString("Ya calificaste: %1★").arg(previo));

    }else{

        // Solo habilitar si ya escucho ≥30 s
        btnCalificar->setEnabled(habilitaRate);

    }

}

void ReproductorMusica::calificarActual()
{

    if(indiceActual<0||indiceActual>=listaCanciones.size())return;
    const Cancion&c=listaCanciones[indiceActual];

    //verificar gating y duplicado para mayor seguridad
    GestorCalificaciones gc;
    quint8 prev=0;
    if(gc.yaCalifico(static_cast<quint32>(usuarioActivo.getId()),static_cast<quint32>(c.getId()),&prev))
    {

        QMessageBox::information(this,"Calificacion",QString("Ya calificaste esta canción con %1★.").arg(prev));
        btnCalificar->setEnabled(false);
        return;

    }

    if(!habilitaRate)
    {

        QMessageBox::warning(this,"Calificacion","Debes escuchar al menos 30 segundos antes de calificar.");
        return;

    }

    // Dialogo simple (1..5)
    QDialog dlg(this);
    dlg.setWindowTitle("Calificar Cancion");

    QVBoxLayout*v=new QVBoxLayout(&dlg);
    QLabel*infoLbl=new QLabel(QString("Canción: <b>%1</b><br/>Artista: %2").arg(c.getTitulo(),c.getNombreArtista()));
    infoLbl->setTextFormat(Qt::RichText);
    v->addWidget(infoLbl);

    QComboBox*cbo=new QComboBox;
    for (int k=1;k<=5;++k)cbo->addItem(QString::number(k)+" ★",k);
    cbo->setCurrentIndex(4); // 5★ por defecto
    v->addWidget(cbo);


    QDialogButtonBox*box=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    v->addWidget(box);
    QObject::connect(box,&QDialogButtonBox::accepted,&dlg,&QDialog::accept);
    QObject::connect(box,&QDialogButtonBox::rejected,&dlg,&QDialog::reject);

    if(dlg.exec()!=QDialog::Accepted)return;

    const quint8 rating =static_cast<quint8>(cbo->currentData().toInt());
    if(!gc.registrar(static_cast<quint32>(usuarioActivo.getId()),static_cast<quint32>(c.getId()),rating))
    {

        QMessageBox::critical(this, "Calificacion","No se pudo guardar la calificacion.");
        return;

    }

    QMessageBox::information(this, "Calificacion",QString("¡Gracias! Registrado: %1★").arg(rating));

    //refresca el promedio y bloquea el boton
    actualizarUIRating(c);

}
