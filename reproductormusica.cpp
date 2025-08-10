#include"reproductormusica.h"
#include<QPixmap>
#include<QStyle>
#include<QDebug>
#include<QFile>
#include<QEventLoop>
#include<QMessageBox>
#include<QMessageBox>

ReproductorMusica::ReproductorMusica(const QVector<Cancion>& canciones, const Usuario& usuario, QWidget *parent): QWidget(parent), listaCanciones(canciones), usuarioActivo(usuario), indiceActual(0)
{

    setWindowTitle("Reproductor Musical");
    setFixedSize(900, 700);
    setStyleSheet("background-color: #121212; color: white;");

    control=new ControlReproduccion(this);
    control->setListaCanciones(listaCanciones);

    //AQUI SE ACTUALIZA LA BARRA DE PROGRESO Y DURACION DEL REPRODUCTOR DE CONTROL
    connect(control->getReproductor(),&QMediaPlayer::positionChanged,this,&ReproductorMusica::actualizarPosicion);
    connect(control->getReproductor(),&QMediaPlayer::durationChanged,this,&ReproductorMusica::actualizarDuracion);

    configurarInterfaz();

    //AL CAMBIAR DE CANCION
    connect(control,&ControlReproduccion::indiceActualizado,this,[=](int indice)
    {

       listaWidget->setCurrentRow(indice);
        const Cancion&c=listaCanciones[indice];

        lblTitulo->setText(c.getTitulo());
        lblArtista->setText(c.getNombreArtista());
        lblTipo->setText(tipoToString(c.getTipo()));
        lblReproducciones->setText(QString::number(c.getReproducciones()) + " Reproducciones");
        lblDescripcion->setText("Descripcion:"+c.getDescripcion());

        if (QFile::exists(c.getRutaImagen())) {
            lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(250,250,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        } else {
            lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(250,250,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        }


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

        }else{

            control->reproducir(listaWidget->currentRow());
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

    bool*repetirActivo=new bool(false);
    bool*aleatorioActivo=new bool(false);

    //BOTON REPETIR
    connect(btnRepetir,&QPushButton::clicked,this,[=]()
    {

        *repetirActivo= !(*repetirActivo);

        //SI SE ACTIVA REPETIR, SE DESACTIVA ALEATORIO
        if(*repetirActivo)
        {

            *aleatorioActivo=false;
            control->activarAleatorio(false);
            btnAleatorio->setStyleSheet("");

        }
        control->activarRepetir(*repetirActivo);
        btnRepetir->setStyleSheet(*repetirActivo?"background-color: #1DB954;": "");

    });

    //BOTON ALEATORIO
    connect(btnAleatorio,&QPushButton::clicked,this,[=]()
    {

        *aleatorioActivo = !(*aleatorioActivo);

        //SI SE ACTIVA ALEATORIO, SE DESACTIVA REPETIR
        if(*aleatorioActivo)
        {

            *repetirActivo=false;
            control->activarRepetir(false);
            btnRepetir->setStyleSheet("");

        }

        control->activarAleatorio(*aleatorioActivo);
        btnAleatorio->setStyleSheet(*aleatorioActivo?"background-color: #1DB954":"");

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

    lblReproducciones=new QLabel("0 Reproduciones");
    lblReproducciones->setStyleSheet("font-size: 16px; color: #b3b3b3;");

    lblDescripcion=new QLabel("Descripcion:");
    lblDescripcion->setStyleSheet("font-size: 14px; color: #b3b3b3;");
    lblDescripcion->setWordWrap(true);
    lblDescripcion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    textoLayout->addWidget(lblTipo);
    textoLayout->addWidget(lblTitulo);
    textoLayout->addWidget(lblArtista);
    textoLayout->addWidget(lblReproducciones);
    textoLayout->addWidget(lblDescripcion);
    textoLayout->addStretch();

    infoLayout->addLayout(textoLayout);
    mainLayout->addLayout(infoLayout);

    // Barra de progreso
    barraProgreso = new SliderClickable(Qt::Horizontal);
    barraProgreso->setRange(0, 0);
    barraProgreso->setStyleSheet("QSlider::groove:horizontal { background: #333; height: 4px; }"
                                 "QSlider::handle:horizontal { background: #ff3333; width: 12px; }");

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

    btnGuardarPlaylist =new QPushButton("➕ Guardar");
    btnGuardarPlaylist->setToolTip("Guardar en playlist");

    btnCerrar =new QPushButton("<-- Volver");
    btnCerrar->setToolTip("Volver al home");

    QList<QPushButton*>botones={btnAleatorio, btnAnterior, btnPlayPause, btnSiguiente, btnRepetir, btnGuardarPlaylist, btnCerrar};
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

    QLabel* lblDuracionEnc = new QLabel("⏱");
    lblDuracionEnc->setStyleSheet("color: #b3b3b3; font-size: 14px;");
    lblDuracionEnc->setAlignment(Qt::AlignRight);
    lblDuracionEnc->setFixedWidth(60);

    encabezadoLayout->addWidget(lblNum);
    encabezadoLayout->addWidget(lblTituloEnc, 1);
    encabezadoLayout->addWidget(lblReproEnc);
    encabezadoLayout->addWidget(lblDuracionEnc);

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
            lblDuracion->setFixedWidth(60);
            lblDuracion->setAlignment(Qt::AlignRight);
            lblDuracion->setStyleSheet("color: #b3b3b3; font-size: 14px;");

            // Agregar al layout
            filaLayout->addWidget(lblNumero);
            filaLayout->addLayout(tituloArtistaLayout, 1);
            filaLayout->addWidget(lblReproducciones);
            filaLayout->addWidget(lblDuracion);

            QListWidgetItem* item = new QListWidgetItem(listaWidget);
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
                lblTitulo->setText(c.getTitulo());
                lblArtista->setText(c.getNombreArtista());
                lblDescripcion->setText("Descripcion:"+c.getDescripcion());

                //VALIDACION SI TIENE CARATULA VALIDA
                if(QFile::exists(c.getRutaImagen()))
                {

                    lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(250,250,Qt::KeepAspectRatio,Qt::SmoothTransformation));

                }else{

                    lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));

                }

            }

        });

    }
    connect(btnCerrar, &QPushButton::clicked, this, &ReproductorMusica::cerrarReproductor);
}

void ReproductorMusica::cerrarReproductor()
{

    Home*h=new Home(usuarioActivo,nullptr);
    this->close();
    this->hide();
    h->show();

}

void ReproductorMusica::actualizarPosicion(qint64 position)
{

    barraProgreso->setValue(position);
    lblTiempoActual->setText(formatoTiempo(position));

}

void ReproductorMusica::actualizarDuracion(qint64 duration)
{

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

    //Si la musica esta sonando, se muestra una advertencia.
    if(control->getReproductor()->playbackState()==QMediaPlayer::PlayingState)
    {

        QMessageBox::StandardButton respuesta=QMessageBox::question(this, "Detener reproduccion","¿Deseas cerrar el reproductor? La musica dejara de escucharse.",QMessageBox::Yes|QMessageBox::No);

        //Si el usuario acepta, se detiene la musica y se cierra.
        if(respuesta==QMessageBox::Yes)
        {

            control->detener();//DETIENE LA MUSICA
            event->accept();//CIERRA LA VENTANA

        }else{//Si no, la ventana no se cierra.

            event->ignore();//CANCELA EL CIERRE

        }

    }else{//Si no esta sonando nada, igual se detiene por si hay musica cargada pero pausada.


        control->detener();//TAMBIEN DETENER SI QUEDO CARGADA
        event->accept();

    }

}
