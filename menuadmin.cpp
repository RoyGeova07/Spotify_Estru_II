#include "menuadmin.h"
#include<QGraphicsDropShadowEffect>
#include<QPainter>
#include<QPainterPath>
#include"menuinicio.h"
#include<QMediaPlayer>
#include<QFileDialog>
#include<QTime>
#include<QUrl>
#include<QMessageBox>
#include<QDate>
#include<QAudioOutput>
#include"gestorcanciones.h"
#include<QScrollArea>
#include"cancion.h"

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
    btnMiMusica=new QPushButton("🎧  Mi Música");
    btnVerEstadisticas=new QPushButton("📊  Ver Estadísticas");
    btnSalir=new QPushButton("🔙  Cerrar Sesión");

    QVector<QPushButton*>botones=
    {

        btnSubirCancion,
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

    layoutDerecho=new QVBoxLayout(panelDerecho);
    layoutDerecho->addStretch();
    layoutDerecho->addWidget(lblMensaje);
    layoutDerecho->addStretch();

    // Ensamble final
    layoutGeneral->addWidget(panelLateral);
    layoutGeneral->addWidget(panelDerecho);

    // Conexiones
    connect(btnSubirCancion, &QPushButton::clicked, this, &MenuAdmin::abrirVentanaSubirCancion);
    connect(btnMiMusica,&QPushButton::clicked,this,&MenuAdmin::MostrarPanelMiMusica);
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

    //Limpiar todo el contenido actual del panel derecho
    LimpiarPanelDerecho();

    QHBoxLayout*layoutSuperior =new QHBoxLayout;
    layoutSuperior->addStretch();

    layoutDerecho->addLayout(layoutSuperior);

    // Nombre del artista
    lblNombreArtista = new QLabel("Artista: " + artista.getNombreArtistico());
    lblNombreArtista->setStyleSheet("font-weight: bold; font-size: 20px; color: #4CAF50;");
    lblNombreArtista->setAlignment(Qt::AlignCenter);

    // Combo tipo de publicación
    QLabel* lblTipo = new QLabel("Tipo de publicación:");
    comboTipo = new QComboBox;
    comboTipo->addItems({"Single", "EP", "Álbum"});
    comboTipo->setCurrentIndex(0);
    connect(comboTipo, &QComboBox::currentTextChanged, this, &MenuAdmin::tipoSeleccionado);

    // Pestañas y botón
    tabsCanciones = new QTabWidget;
    crearPestañasCanciones(1); // Por defecto single

    btnSubir = new QPushButton("🎵 Subir Canciones");
    btnSubir->setFixedHeight(40);
    btnSubir->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 10px; font-size: 16px; }"
                            "QPushButton:hover { background-color: #388E3C; }");
    connect(btnSubir, &QPushButton::clicked, this, &MenuAdmin::subirCanciones);

    // Insertar al panel
    layoutDerecho->addWidget(lblNombreArtista);
    layoutDerecho->addSpacing(10);
    layoutDerecho->addWidget(lblTipo);
    layoutDerecho->addWidget(comboTipo);
    layoutDerecho->addWidget(tabsCanciones);
    layoutDerecho->addSpacing(10);
    layoutDerecho->addWidget(btnSubir);
}

void MenuAdmin::CerrarSesion()
{

    MenuInicio*m=new MenuInicio(nullptr);
    m->show();
    this->close();

}

void MenuAdmin::tipoSeleccionado(const QString &tipo)
{

    int cantidad=1;
    if(tipo=="EP")cantidad=3;
    else if(tipo=="Álbum")cantidad=8;

    crearPestañasCanciones(cantidad);

}

void MenuAdmin::crearPestañasCanciones(int cantidad)
{


    tabsCanciones->clear();
    cancionesWidgets.clear();

    GestorCanciones gestor;
    int IDinicial=gestor.generarId();//solo se generara una vez

    for(int i=0;i<cantidad;++i)
    {

        int idActual=IDinicial+i;
        QWidget*pestania=crearFormularioCancion(i,idActual);
        tabsCanciones->addTab(pestania,"Cancion"+QString::number(i+1));

    }

}

QWidget* MenuAdmin::crearFormularioCancion(int /*index*/, int ID)
{

    QWidget*contenido=new QWidget;
    WidgetCancion w;

    contenido->setStyleSheet(R"(
        QLabel {
            font-weight: bold;
            font-size: 13px;
            color: white;
        }

        QLineEdit, QTextEdit, QComboBox {
            background-color: #2c2c2c;
            color: white;
            border: 1px solid #555;
            border-radius: 5px;
            padding: 4px;
            font-size: 13px;
        }

        QLineEdit:read-only {
            background-color: #1e1e1e;
            color: #aaa;
        }

        QTextEdit {
            min-height: 40px;
            max-height: 60px;
        }

        QPushButton {
            background-color: #4CAF50;
            color: white;
            border-radius: 10px;
            font-size: 14px;
            padding: 6px;
        }

        QPushButton:hover {
            background-color: #388E3C;
        }

        QTabWidget::pane {
            border: 1px solid #4CAF50;
            border-radius: 6px;
            margin-top: 6px;
        }

        QTabBar::tab {
            background: #333;
            color: white;
            padding: 4px 12px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }

        QTabBar::tab:selected {
            background: #4CAF50;
        }

        QScrollBar:vertical {
            background: #2c2c2c;
            width: 10px;
            margin: 0px;
        }

        QScrollBar::handle:vertical {
            background: #4CAF50;
            border-radius: 4px;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            background: none;
            height: 0px;
        }
    )");

    w.lblIdCancion=new QLabel(QString("ID asignado %1").arg(ID));
    w.lblIdCancion->setStyleSheet("font-size: 14px; color: #4CAF50");
    w.txtTitulo=new QLineEdit;
    w.txtGenero=new QComboBox;
    w.txtGenero->addItems({"Pop","Corridos","Cristianos","Electronica","Regueton","Rock","Clasicas"});

    w.txtDuracion=new QLineEdit;
    w.txtDuracion->setReadOnly(true);//solo lectura paaaaa

    w.comboCategoria=new QComboBox;
    w.comboCategoria->addItems({"playlist", "recomendado", "favorito", "infantil", "instrumental"});

    w.txtDescripcion=new QTextEdit;

    w.txtRutaAudio=new QLineEdit;
    w.txtRutaAudio->setReadOnly(true);

    w.txtRutaImagen=new QLineEdit;
    w.txtRutaImagen->setReadOnly(true);

    w.btnSeleccionarAudio=new QPushButton("Seleccionar Audio");
    w.btnSeleccionarImagen=new QPushButton("Seleccionar Imagen");
    w.vistaPreviaImagen=new VistaPreviaImagen;
    w.vistaPreviaImagen->setFixedSize(160,160);
    w.vistaPreviaImagen->setStyleSheet("border: 1px solid #555; background-color: #111; color: white;");
    w.vistaPreviaImagen->setAlignment(Qt::AlignCenter);
    w.vistaPreviaImagen->setText("Sin Imagen");

    connect(w.btnSeleccionarAudio,&QPushButton::clicked,this,&MenuAdmin::seleccionarAudio);
    connect(w.btnSeleccionarImagen,&QPushButton::clicked,this,&MenuAdmin::SeleccionarImagen);

    cancionesWidgets.append(w);

    QVBoxLayout*layout= new QVBoxLayout(contenido);
    layout->setSpacing(6);

    layout->addWidget(w.lblIdCancion);
    layout->addWidget(new QLabel("Título:"));
    layout->addWidget(w.txtTitulo);

    layout->addWidget(new QLabel("Género:"));
    layout->addWidget(w.txtGenero);

    layout->addWidget(new QLabel("Duración:"));
    layout->addWidget(w.txtDuracion);

    layout->addWidget(new QLabel("Descripción:"));
    layout->addWidget(w.txtDescripcion);

    layout->addWidget(new QLabel("Categoría:"));
    layout->addWidget(w.comboCategoria);

    layout->addWidget(new QLabel("Ruta Audio:"));
    layout->addWidget(w.txtRutaAudio);
    layout->addWidget(w.btnSeleccionarAudio);

    layout->addWidget(new QLabel("Ruta Imagen:"));
    layout->addWidget(w.txtRutaImagen);
    layout->addWidget(w.btnSeleccionarImagen);

    //CENTRO LA VISTA PREVIA DE LA IMAGEN DE LA MINIATURA
    QHBoxLayout*contenedorImagen=new QHBoxLayout;
    contenedorImagen->addStretch();
    contenedorImagen->addWidget(w.vistaPreviaImagen);
    contenedorImagen->addStretch();
    layout->addLayout(contenedorImagen);//MINIUATURA MINIATURAAAAAAAA

    //AGREGO SCROLL PARA MEJOR ESTETICA EN EL FRONTED
    QScrollArea*scroll=new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(contenido);
    scroll->setStyleSheet("QScrollArea { border: none; }");

    return scroll;

}

void MenuAdmin::seleccionarAudio()
{

    int indice=tabsCanciones->currentIndex();//pestania actual
    if(indice<0||indice>=cancionesWidgets.size())return;

    QString ruta=QFileDialog::getOpenFileName(this,"Seleccionar archivo de audio","","Audio(*.mp3 *.mp4 *.m4a)");
    if(!ruta.isEmpty())
    {

        cancionesWidgets[indice].txtRutaAudio->setText(ruta);

        //aqui calcula la duracion
        QMediaPlayer*player=new QMediaPlayer(this);
        QAudioOutput*audioOutput=new QAudioOutput(this);
        player->setAudioOutput(audioOutput);
        player->setSource(QUrl::fromLocalFile(ruta));

        connect(player,&QMediaPlayer::durationChanged,this,[=](qint64 duracionMS){

            QString duracion=QTime(0,0).addMSecs(duracionMS).toString("hh:mm:ss");
            cancionesWidgets[indice].txtDuracion->setText(duracion);
            player->deleteLater();
            audioOutput->deleteLater();

        });
        player->play();//se usa para forzar que lea la duracion

    }

}

void MenuAdmin::SeleccionarImagen()
{

    int indice=tabsCanciones->currentIndex();//pestania actual
    if(indice<0||indice>=cancionesWidgets.size())return;

    QString ruta=QFileDialog::getOpenFileName(this,"Seleccionar Imagen","","Imágenes (*.jpg *.jpeg *.png)");
    if(!ruta.isEmpty())
    {

        cancionesWidgets[indice].txtRutaImagen->setText(ruta);

        //aqui se carga la vista previa
        QPixmap pixmap(ruta);
        if(!pixmap.isNull())
        {

            cancionesWidgets[indice].vistaPreviaImagen->setPixmap(pixmap.scaled(cancionesWidgets[indice].vistaPreviaImagen->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));

        }else{

            cancionesWidgets[indice].vistaPreviaImagen->setText("Vista no disponible");

        }

    }else{

        //si cancela el artista, limpiar la miniatura y poner texto
        cancionesWidgets[indice].vistaPreviaImagen->setPixmap(QPixmap());
        cancionesWidgets[indice].vistaPreviaImagen->setText("Sin Imagen");

    }

}

void MenuAdmin::subirCanciones()
{

    GestorCanciones gestor;
    Tipo tipoEnum=stringToTipo(comboTipo->currentText());

    //aqui se obtiene el primer ID disponible de una sola vez
    int IDInicial=gestor.generarId();

    QVector<Cancion>cancionesAGuardar;

    //Se usaa QSet<QString> para llevar control de los titulos y rutas ya ingresados en el formulario
    QSet<QString> titulosUnicos;
    QSet<QString> rutasAudioUnicas;

    for(int i=0;i<cancionesWidgets.size();i++)
    {

        const WidgetCancion& w = cancionesWidgets[i];

        QString titulo=w.txtTitulo->text().trimmed();
        QString genero=w.txtGenero->currentText();
        QString duracion=w.txtDuracion->text().trimmed();
        QString descripcion=w.txtDescripcion->toPlainText().trimmed();
        QString categoria=w.comboCategoria->currentText();
        QString rutaAudio=w.txtRutaAudio->text().trimmed();
        QString rutaImagen=w.txtRutaImagen->text().trimmed();

        if(titulo.isEmpty()||genero.isEmpty()||duracion.isEmpty()||descripcion.isEmpty()||categoria.isEmpty()||rutaAudio.isEmpty()||rutaImagen.isEmpty())
        {

            QMessageBox::warning(this, "Campos incompletos",QString("Faltan datos en la canción %1. Por favor, completa todos los campos.").arg(i+1));
            return;

        }

        //aqui se verifica duplicados internos, dentro del formulaario
        if(titulosUnicos.contains(titulo))
        {

            QMessageBox::warning(this,"Titulo duplicado",QString("El título '%1' esta repetido dentro del mismo formulario.").arg(titulo));
            return;

        }

        if(rutasAudioUnicas.contains(rutaAudio))
        {

            QMessageBox::warning(this,"Ruta de audio duplicada",QString("La ruta de audio '%1' esta repetida dentro del mismo formulario.").arg(rutaAudio));
            return;

        }

        titulosUnicos.insert(titulo);
        rutasAudioUnicas.insert(rutaAudio);

        Genero g=stringToGenero(genero);
        Categoria c=stringToCategoria(categoria);

        //Aqui se asgina el id
        int id=IDInicial+i;
        Cancion cancion(0,id, titulo, artista.getNombreArtistico(), g, c, tipoEnum, descripcion,rutaAudio, rutaImagen, duracion, QDate::currentDate(), true);

        if(gestor.CancionDuplicada(cancion))
        {

            QMessageBox::warning(this,"Cancion Duplicada",QString("La cancion %1 no se puede guardar porque ya existe una cancion con el mismo titulo, audio o imagen.").arg(i+1));
            return;

        }

        cancionesAGuardar.append(cancion);// solo si pasa toda la validacion

    }

    //SE EVITA LOS DUPLICADOS
    int CancionesGuardadas=0;
    for(const Cancion& c:cancionesAGuardar)
    {

        if(gestor.guardarCancion(c))
        {

            ++CancionesGuardadas;

        }

    }

    if(CancionesGuardadas==cancionesAGuardar.size())
    {

        QMessageBox::information(this,"Exito","¡Todas las canciones se guardaron correctamente!");
        tipoSeleccionado(comboTipo->currentText());//aqui se reinicia las pestañas

    }

}

void MenuAdmin::LimpiarPanelDerecho()
{


    while(QLayoutItem*item=layoutDerecho->takeAt(0))
    {

        if(QWidget*widget=item->widget())
        {

            widget->deleteLater();  // Eliminar el widget asociado

        }
        delete item;  // Eliminar el layout item
    }


}

void MenuAdmin::MostrarPanelMiMusica()
{

    LimpiarPanelDerecho();
    // ELIMINAR panel anterior si existía
    if(panelReproductorAdmin)
    {

        panelReproductorAdmin->deleteLater();
        panelReproductorAdmin=nullptr;

    }

    if(controlAdmin)
    {

        controlAdmin->deleteLater();
        controlAdmin=nullptr;

    }


    controlAdmin=new ControlReproduccion(this);

    connect(controlAdmin, &ControlReproduccion::indiceActualizado, this, &MenuAdmin::mostrarDatosCancionActual);
    connect(controlAdmin->getReproductor(), &QMediaPlayer::positionChanged,this,&MenuAdmin::actualizarTiempo);
    connect(controlAdmin->getReproductor(),&QMediaPlayer::durationChanged,this,&MenuAdmin::actualizarDuracion);

    // ========== Panel y layout principal ==========
    QWidget *panelReproductor=new QWidget;
    QVBoxLayout *layout=new QVBoxLayout(panelReproductor);
    layout->setContentsMargins(20,20,20,20);

    QLabel *lblUsuario = new QLabel("Administrador: " + artista.getNombreArtistico());
    lblUsuario->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    layout->addWidget(lblUsuario, 0, Qt::AlignLeft);

    // ========== Imagen y datos ==========
    QHBoxLayout *layoutTop=new QHBoxLayout;

    lblCaratula=new QLabel;
    lblCaratula->setFixedSize(200,200);
    lblCaratula->setStyleSheet("border: 2px solid white; background-color: #333;");
    layoutTop->addWidget(lblCaratula);

    QVBoxLayout*layoutInfo=new QVBoxLayout;
    lblTitulo =new QLabel("Título");
    lblTitulo->setStyleSheet("color: white; font-size: 18px;");
    lblArtista= new QLabel("Artista");
    lblArtista->setStyleSheet("color: gray; font-size: 14px;");
    lblTipo = new QLabel("Tipo");
    lblTipo->setStyleSheet("color: gray;");
    lblReproducciones = new QLabel("Reproducciones: 0");
    lblReproducciones->setStyleSheet("color: gray;");
    layoutInfo->addWidget(lblTitulo);
    layoutInfo->addWidget(lblArtista);
    layoutInfo->addWidget(lblTipo);
    layoutInfo->addWidget(lblReproducciones);

    layoutTop->addLayout(layoutInfo);
    layout->addLayout(layoutTop);

    // ========== Barra de progreso ==========
    barraProgreso = new SliderClickable(Qt::Horizontal);
    barraProgreso->setRange(0,100);
    barraProgreso->setStyleSheet("QSlider::groove:horizontal { background: gray; height: 6px; }"
                                 "QSlider::handle:horizontal { background: white; width: 12px; border-radius: 6px; }");
    layout->addWidget(barraProgreso);

    // ========== Tiempos ==========
    QHBoxLayout *layoutTiempos=new QHBoxLayout;
    lblTiempoActual=new QLabel("00:00");
    lblTiempoActual->setStyleSheet("color: white;");
    lblDuracionTotal=new QLabel("00:00");
    lblDuracionTotal->setStyleSheet("color: white;");
    layoutTiempos->addWidget(lblTiempoActual);
    layoutTiempos->addStretch();
    layoutTiempos->addWidget(lblDuracionTotal);
    layout->addLayout(layoutTiempos);

    // ========== Controles ==========
    QHBoxLayout *layoutControles = new QHBoxLayout;
    btnAnterior=new QPushButton("⏮️");
    btnPlayPause=new QPushButton("▶️");
    btnSiguiente= new QPushButton("⏭️");
    btnRepetir =new QPushButton("🔁");
    btnAleatorio= new QPushButton("🔀");
    btnEditarCancionReproductor = new QPushButton("✏️");
    btnEliminarCancionReproductor = new QPushButton("🗑️");

    QList<QPushButton *> botones = {
        btnAnterior, btnPlayPause, btnSiguiente,
        btnRepetir, btnAleatorio, btnEditarCancionReproductor, btnEliminarCancionReproductor
    };

    for (auto btn : botones) {
        btn->setFixedSize(50, 50);
        btn->setStyleSheet("background-color: #222; color: white; border-radius: 25px;");
        layoutControles->addWidget(btn);
    }
    layout->addLayout(layoutControles);

    // ========== Lista de canciones ==========
    listaWidget = new QListWidget;
    listaWidget->setStyleSheet("QListWidget { background-color: #121212; color: white; }"
                               "QListWidget::item:selected { background-color: #1DB954; color: black; }");

    layout->addWidget(listaWidget);
    layoutDerecho->addWidget(panelReproductor);

    // ========== Cargar canciones propias ==========
    GestorCanciones gestor;
    QVector<Cancion>todas=gestor.leerCanciones();
    QVector<Cancion>propias;

    for(const Cancion&c:todas)
    {

        if(c.getNombreArtista()==artista.getNombreArtistico())
        {

            propias.append(c);
            listaWidget->addItem(c.getTitulo());

        }

    }

    listaCanciones=propias;
    controlAdmin->setListaCanciones(listaCanciones);

    // ========== Cambio de seleccion ==========
    connect(listaWidget, &QListWidget::currentRowChanged, this, [=](int index)
    {

        if(index>=0&&index<listaCanciones.size())
        {

            const Cancion &c=listaCanciones[index];

            lblTitulo->setText(c.getTitulo());
            lblArtista->setText(c.getNombreArtista());
            lblTipo->setText(tipoToString(c.getTipo()));
            lblReproducciones->setText("Reproducciones: " + QString::number(c.getReproducciones()));
            lblDuracionTotal->setText(c.getDuracion().isEmpty() ? "00:00" : c.getDuracion());

            if (QFile::exists(c.getRutaImagen()))
            {

                lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            }else{

                lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

            }
        }
    });

    //Seleccion inicial
    if (!listaCanciones.isEmpty()) {
        listaWidget->setCurrentRow(0);
    }

    //Reproducir al hacer clic
    connect(listaWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem *item)
    {

        int index=listaWidget->row(item);
        controlAdmin->reproducir(index);
        btnPlayPause->setText("⏸️");

    });

    // Play/Pause manual
    connect(btnPlayPause, &QPushButton::clicked, this, [=]()
    {

        QMediaPlayer*rep=controlAdmin->getReproductor();
        if(rep->playbackState() == QMediaPlayer::PlayingState)
        {

            rep->pause();
            btnPlayPause->setText("▶️");

        }else{

            controlAdmin->reproducir(listaWidget->currentRow());
            btnPlayPause->setText("⏸️");

        }

    });

    connect(barraProgreso, &QSlider::sliderReleased, this, [=]()
    {
        if(controlAdmin&&controlAdmin->getReproductor())
        {

            qint64 nuevaPos=barraProgreso->value();
            controlAdmin->getReproductor()->setPosition(nuevaPos);

        }
    });

    //BOTON SIGUIENTE
    connect(btnSiguiente, &QPushButton::clicked, this, [=]()
    {

        controlAdmin->siguiente();
        btnPlayPause->setText("⏸️");

    });

    //BOTON ANTERIOR
    connect(btnAnterior, &QPushButton::clicked, this, [=]()
    {

        controlAdmin->anterior();
        btnPlayPause->setText("⏸️");

    });

    //BOTON REPETIR
    connect(btnRepetir, &QPushButton::clicked, this, [=]()
    {

        static bool activo=false;
        activo=!activo;
        controlAdmin->activarRepetir(activo);

        btnRepetir->setStyleSheet(activo ?
                                      "background-color: #1DB954; color: black; border-radius: 25px;" :
                                      "background-color: #222; color: white; border-radius: 25px;");

    });

    //BOTON ALEATORIO
    connect(btnAleatorio, &QPushButton::clicked, this, [=]()
    {
        static bool activo=false;
        activo=!activo;
        controlAdmin->activarAleatorio(activo);

        btnAleatorio->setStyleSheet(activo ?
                                        "background-color: #1DB954; color: black; border-radius: 25px;" :
                                        "background-color: #222; color: white; border-radius: 25px;");
    });

    //BOTON DE ELIMINAR CUENTA
    //DESPUES LO HAGO

}

QString MenuAdmin::formatearTiempo(qint64 milisegundos)
{

    int segundos=milisegundos/1000;
    int minutos=segundos/60;
    segundos%=60;
    return QString("%1:%2")
        .arg(minutos,2,10,QChar('0'))
        .arg(segundos,2,10,QChar('0'));

}

void MenuAdmin::actualizarTiempo(qint64 posicion)
{

    if(lblTiempoActual&&barraProgreso&&controlAdmin&&controlAdmin->getReproductor())
    {

        lblTiempoActual->setText(formatearTiempo(posicion));
        barraProgreso->setValue(posicion);

    }

}

void MenuAdmin::actualizarDuracion(qint64 duracion)
{

    if(lblDuracionTotal && barraProgreso)
    {

        lblDuracionTotal->setText(formatearTiempo(duracion));
        barraProgreso->setMaximum(duracion);

    }

}

void MenuAdmin::mostrarDatosCancionActual(int index)
{

    if(index>=0&&index<listaCanciones.size())
    {

        const Cancion&c=listaCanciones[index];

        lblTitulo->setText(c.getTitulo());
        lblArtista->setText(c.getNombreArtista());
        lblTipo->setText(tipoToString(c.getTipo()));
        lblReproducciones->setText("Reproducciones: "+QString::number(c.getReproducciones()));
        lblDuracionTotal->setText(c.getDuracion().isEmpty()?"00:00":c.getDuracion());

        if(QFile::exists(c.getRutaImagen()))
        {

            lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        }else{

            lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        }

        listaWidget->setCurrentRow(index);  //SE asegura que se refleje en la lista
    }

}
