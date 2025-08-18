#include "menuadmin.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include "menuinicio.h"
#include <QMediaPlayer>
#include <QFileDialog>
#include <QTime>
#include <QUrl>
#include <QMessageBox>
#include <QDate>
#include <QAudioOutput>
#include "gestorcanciones.h"
#include <QScrollArea>
#include "cancion.h"
#include <QInputDialog>
#include "gestorartistas.h"
#include <QTableWidget>
#include <QProgressBar>
#include "tipos.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QSet>
#include"gestorreproduccion.h"
#include"gestorusuarios.h"
#include<limits>
#include<QStyledItemDelegate>
#include"gestorcalificaciones.h"
#include<QDir>

// Centra todo el texto de todas las celdas de las tablas
namespace
{

class CenterDelegate:public QStyledItemDelegate
{

public:

    using QStyledItemDelegate::QStyledItemDelegate;

    void initStyleOption(QStyleOptionViewItem *opt, const QModelIndex &idx) const override
    {

        QStyledItemDelegate::initStyleOption(opt, idx);
        opt->displayAlignment=Qt::AlignCenter; // centro horizontal y vertical

    }

};

} // namespace


// Busca por songId en TODAS las playlists de TODOS los usuarios (Publico/Usuario_*/ *.dat)
// y devuelve Titulo/Artista si lo encuentra (formato v2 con id guardado).
static bool metaDesdeCualquierPlaylist(quint32 songId, QString* outTitulo, QString* outArtista)
{

    QDir raiz("Publico");
    if(!raiz.exists())return false;

    const QStringList carpetasUsuarios=raiz.entryList(QStringList()<<"Usuario_*",QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name);
    for(const QString& carpeta:carpetasUsuarios)
    {

        QDir dirUsuario(raiz.filePath(carpeta));
        const QStringList archivos=dirUsuario.entryList(QStringList()<<"*.dat", QDir::Files, QDir::Name);
        for (const QString& f : archivos)
        {

            QFile in(dirUsuario.filePath(f));
            if(!in.open(QIODevice::ReadOnly))continue;

            QDataStream ds(&in);
            ds.setVersion(QDataStream::Qt_5_15);

            while(!ds.atEnd())
            {
                quint32 magic=0; quint16 ver=0;
                ds>>magic>>ver;
                if(ds.status()!=QDataStream::Ok)break;
                if(magic != 0x534F4E47)break; //'SONG'

                quint32 id=0;
                QString titulo, artista,descripcion,rutaAudio,rutaImagen,duracionStr;
                quint16 tipo =0, genero= 0;
                QDate fecha;
                qint32 reproducciones =0;
                bool activa =false;

                // Formato v2: primero viene el id (es lo que nos habilita este fallback)
                if(ver>=2)
                {

                    ds>>id;

                }
                ds>>titulo;
                ds>>artista;
                ds>>tipo;
                ds>>descripcion;
                ds>>genero;
                ds>>fecha;
                ds>>duracionStr;
                ds>>rutaAudio;
                ds>>rutaImagen;
                ds>>reproducciones;
                ds>>activa;

                if(ver>=2&&id==songId)
                {

                    if(outTitulo)*outTitulo=titulo;
                    if(outArtista)*outArtista=artista;
                    return true;

                }
            }
        }
    }
    return false;
}

MenuAdmin::MenuAdmin(const Artista& artistaActivo, QWidget *parent): QWidget(parent), artista(artistaActivo)
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
    if(!pix.isNull()){
        int lado=qMin(pix.width(), pix.height());
        QRect centro((pix.width()-lado)/2,(pix.height()-lado)/2,lado,lado);
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
    btnPerfil=new QPushButton("🎤 Mi perfil");

    QVector<QPushButton*>botones = { btnSubirCancion, btnMiMusica, btnVerEstadisticas, btnPerfil, btnSalir };
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

    // ======================== PANEL DERECHO =========================
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
    connect(btnSubirCancion,&QPushButton::clicked,this,&MenuAdmin::abrirVentanaSubirCancion);
    connect(btnMiMusica,&QPushButton::clicked,this,&MenuAdmin::MostrarPanelMiMusica);
    connect(btnSalir,&QPushButton::clicked,this,&MenuAdmin::CerrarSesion);
    connect(btnPerfil,&QPushButton::clicked,this,&MenuAdmin::MostrarPerfil);
    connect(btnVerEstadisticas,&QPushButton::clicked,this,&MenuAdmin::MostrarEstadisticas);
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
    // Limpiar panel
    LimpiarPanelDerecho();
    ApagarReproductor();

    // ===== Scroll contenedor para que nada se corte =====
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; }");

    auto *content=new QWidget;
    auto *root =new QVBoxLayout(content);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    // ===== Encabezado =====
    lblNombreArtista= new QLabel("Artista: " + artista.getNombreArtistico());
    lblNombreArtista->setStyleSheet("font-weight: bold; font-size: 20px; color: #4CAF50;");
    lblNombreArtista->setAlignment(Qt::AlignCenter);

    QLabel*lblTipo= new QLabel("Tipo de publicación:");
    comboTipo =new QComboBox;

    comboTipo->setStyleSheet(
        // Caja y texto
        "QComboBox {"
        "  background:#232323;"
        "  color:#ffffff;"
        "  border:1px solid #4CAF50;"
        "  border-radius:6px;"
        "  padding:6px 10px;"
        "  min-width:160px;"
        "}"
        "QComboBox:hover { border-color:#22c75f; }"
        "QComboBox:focus { border-color:#22c75f; }"
        "QComboBox::drop-down {"
        "  subcontrol-origin: padding;"
        "  subcontrol-position: top right;"
        "  width:28px;"
        "  border-left:1px solid #4CAF50;"
        "}"
        "QComboBox::down-arrow {"
        "  border-left:6px solid transparent;"
        "  border-right:6px solid transparent;"
        "  border-top:8px solid #4CAF50;"
        "  width: 0px;"
        "  height: 0px;"
        "  margin-right: 8px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background:#1f1f1f;"
        "  color:#ffffff;"
        "  border:1px solid #4CAF50;"
        "  selection-background-color:#2a2a2a;"
        "}"
        );

    comboTipo->addItems({"Single", "EP", "Álbum"});
    comboTipo->setCurrentIndex(0);
    connect(comboTipo, &QComboBox::currentTextChanged, this, &MenuAdmin::tipoSeleccionado);

    auto *rowTipo =new QWidget;
    auto *ltRowTipo= new QHBoxLayout(rowTipo);
    ltRowTipo->setContentsMargins(0,0,0,0);
    ltRowTipo->addWidget(lblTipo);
    ltRowTipo->addWidget(comboTipo, 1);
    ltRowTipo->addStretch();

    // ===== Box de datos globales (EP/Album) =====
    auto* boxGlobal= new QGroupBox("Datos de la publicación (se aplican a todas las pistas)");
    boxGlobal->setStyleSheet(
        "QGroupBox {"
        "  color:#ffffff; font-weight:bold;"
        "  border:1px solid #4CAF50; border-radius:8px;"
        //Deja espacio arriba para que el titulo no se mezcle con el borde
        "  margin-top:16px;"
        "}"
        "QGroupBox::title {"
        //Dibuja el titulo sobre el margen, arriba a la izquierda
        "  subcontrol-origin: margin; subcontrol-position: top left;"
        "  left:12px; padding:0 6px;"
        //Fondo para que el texto no 'se tape' con el borde
        "  background:#181818;"
        "}"
        "QLabel { color:#ddd; }"
        "QLineEdit, QPlainTextEdit, QComboBox {"
        "  background:#232323; color:#fff; border:1px solid #555; border-radius:6px; padding:6px;"
        "}"
        "QPushButton { background:#2e7d32; color:white; border:none; border-radius:8px; padding:8px 12px; }"
        "QPushButton:hover { background:#388E3C; }"
        );
    auto* g =new QGridLayout(boxGlobal);
    g->setContentsMargins(12,12,12,12);
    g->setHorizontalSpacing(10);
    g->setVerticalSpacing(10);

    // Nombre de publicación
    g->addWidget(new QLabel("Nombre de la publicación:"),0,0);
    txtNombrePublicacion = new QLineEdit;
    txtNombrePublicacion->setPlaceholderText("Ej.: Mi EP 2025 / Mi Álbum 2025");
    g->addWidget(txtNombrePublicacion, 0,1,1,3);

    // Genero y categoria globales
    g->addWidget(new QLabel("Género (global):"),1,0);
    comboGeneroGlobal = new QComboBox;
    comboGeneroGlobal->addItems({"Pop","Corridos","Cristianos","Electronica","Regueton","Rock","Clasicas"});
    g->addWidget(comboGeneroGlobal,1,1);

    g->addWidget(new QLabel("Categoría (global):"), 1, 2);
    comboCategoriaGlobal = new QComboBox;
    comboCategoriaGlobal->addItems({"playlist","recomendado","favorito","infantil","instrumental"});
    g->addWidget(comboCategoriaGlobal, 1, 3);

    // Portada unica (izq: label+lineEdit+botón, der: preview)
    g->addWidget(new QLabel("Portada (una para todo):"), 2, 0);
    txtRutaImagenGlobal = new QLineEdit;
    txtRutaImagenGlobal->setReadOnly(true);
    g->addWidget(txtRutaImagenGlobal, 2, 1);

    auto* btnSelImagenGlobal = new QPushButton("Seleccionar portada");
    btnSelImagenGlobal->setMinimumWidth(180);
    connect(btnSelImagenGlobal, &QPushButton::clicked, this, &MenuAdmin::seleccionarImagenGlobal);
    g->addWidget(btnSelImagenGlobal, 2, 2);

    vistaPortadaGlobal = new VistaPreviaImagen;
    vistaPortadaGlobal->setFixedSize(140,140);
    vistaPortadaGlobal->setStyleSheet("border:1px solid #555; background:#111; color:#bbb; border-radius:8px;");
    vistaPortadaGlobal->setAlignment(Qt::AlignCenter);
    vistaPortadaGlobal->setText("Sin imagen");
    g->addWidget(vistaPortadaGlobal, 0, 3, 3, 1); // ocupa 3 filas a la derecha

    // Descripcion global
    g->addWidget(new QLabel("Descripción (global):"),3,0);
    txtDescripcionGlobal = new QPlainTextEdit;
    txtDescripcionGlobal->setPlaceholderText("Escribe aquí la descripción del EP / Álbum (opcional)");
    txtDescripcionGlobal->setFixedHeight(90);
    g->addWidget(txtDescripcionGlobal,3,1,1,3);

    // Botón audios múltiples - toda la fila
    btnSeleccionarAudios = new QPushButton("Seleccionar audios (múltiple)");
    btnSeleccionarAudios->setMinimumHeight(36);
    btnSeleccionarAudios->setCursor(Qt::PointingHandCursor);
    connect(btnSeleccionarAudios, &QPushButton::clicked, this, &MenuAdmin::seleccionarAudiosMultiples);
    g->addWidget(btnSeleccionarAudios,4,0,1,4);

    // Reparto de columnas para que no se amontone
    g->setColumnStretch(0,0); // etiquetas
    g->setColumnStretch(1,1); // campos
    g->setColumnStretch(2,0); // boton pequeño
    g->setColumnStretch(3,1); // preview

    // ===== Pestañas de canciones =====
    tabsCanciones = new QTabWidget;
    tabsCanciones->setMinimumHeight(320);
    tabsCanciones->setStyleSheet(
        "QTabWidget::pane { border:1px solid #333; border-radius:8px; }"
        "QTabBar::tab { background:#202020; color:#ddd; padding:8px 14px; border-top-left-radius:6px; border-top-right-radius:6px; }"
        "QTabBar::tab:selected { background:#2a2a2a; color:#fff; }"
        );
    crearPestañasCanciones(1); // por defecto single

    //===== Boton Subir grande centrado =====
    btnSubir = new QPushButton("🎵  Subir Canciones");
    btnSubir->setFixedHeight(44);
    btnSubir->setCursor(Qt::PointingHandCursor);
    btnSubir->setStyleSheet(
        "QPushButton { background-color: #1DB954; color: #111; font-weight: 700; font-size: 16px; border: none; border-radius: 10px; }"
        "QPushButton:hover { background-color: #22c75f; }"
        );
    connect(btnSubir, &QPushButton::clicked, this, &MenuAdmin::subirCanciones);

    // ===== Ensamblar en el root =====
    root->addWidget(lblNombreArtista);   // 1)Nombre de artista ARRIBA
    root->addWidget(rowTipo);            // 2)Tipo de publicacion debajo

    root->addSpacing(8);
    root->addWidget(boxGlobal);          // 3)Datos globales de EP/Álbum

    root->addSpacing(8);
    root->addWidget(tabsCanciones);      // 4)Pestañas de canciones

    root->addSpacing(6);
    root->addWidget(btnSubir);           // 5)Boton guardar
    scroll->setWidget(content);
    layoutDerecho->addWidget(scroll);

}

void MenuAdmin::CerrarSesion()
{

    ApagarReproductor();
    MenuInicio*m=new MenuInicio(nullptr);
    m->show();
    this->close();

}

void MenuAdmin::tipoSeleccionado(const QString &tipo)
{
    int cantidad=1;
    if(tipo=="EP")      cantidad=3;
    else if(tipo=="Álbum") cantidad=8;
    crearPestañasCanciones(cantidad);
}

void MenuAdmin::crearPestañasCanciones(int cantidad)
{
    tabsCanciones->clear();
    cancionesWidgets.clear();

    GestorCanciones gestor;
    int IDinicial=gestor.generarId(); // se genera una vez

    for(int i=0;i<cantidad;++i){
        int idActual=IDinicial+i;
        QWidget* pestania = crearFormularioCancion(i,idActual);
        tabsCanciones->addTab(pestania, "Cancion "+QString::number(i+1));
    }
}

QWidget* MenuAdmin::crearFormularioCancion(int /*index*/, int ID)
{
    QWidget* contenido = new QWidget;
    WidgetCancion w;

    contenido->setStyleSheet(R"(
        QLabel { font-weight:bold; font-size:13px; color:white; }
        QLineEdit, QTextEdit, QComboBox {
            background-color:#2c2c2c; color:white; border:1px solid #555;
            border-radius:5px; padding:4px; font-size:13px;
        }
        QLineEdit:read-only { background-color:#1e1e1e; color:#aaa; }
    )");

    w.lblIdCancion = new QLabel(QString("ID asignado %1").arg(ID));
    w.lblIdCancion->setStyleSheet("font-size:14px; color:#4CAF50");

    w.txtTitulo   = new QLineEdit;
    w.txtDuracion = new QLineEdit; w.txtDuracion->setReadOnly(true);
    w.txtRutaAudio= new QLineEdit; w.txtRutaAudio->setReadOnly(true);

    cancionesWidgets.append(w);

    auto* layout = new QVBoxLayout(contenido);
    layout->setSpacing(6);

    layout->addWidget(w.lblIdCancion);
    layout->addWidget(new QLabel("Título (pista):"));
    layout->addWidget(w.txtTitulo);

    layout->addWidget(new QLabel("Duración:"));
    layout->addWidget(w.txtDuracion);

    layout->addWidget(new QLabel("Ruta audio:"));
    layout->addWidget(w.txtRutaAudio);

    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(contenido);
    scroll->setStyleSheet("QScrollArea { border:none; }");

    return scroll;
}

void MenuAdmin::seleccionarAudio()
{

    int indice=tabsCanciones->currentIndex();
    if(indice<0||indice>=cancionesWidgets.size()) return;

    QString ruta=QFileDialog::getOpenFileName(this,"Seleccionar archivo de audio","","Audio(*.mp3 *.mp4 *.m4a)");
    if(ruta.isEmpty()) return;

    cancionesWidgets[indice].txtRutaAudio->setText(ruta);

    // Calcula duraciin (silencioso)
    QMediaPlayer* player= new QMediaPlayer(this);
    QAudioOutput* audioOutput =new QAudioOutput(this);
    audioOutput->setVolume(0); // <- que no suene
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(ruta));

    connect(player,&QMediaPlayer::durationChanged,this,[=](qint64 duracionMS)
    {

        QString duracion=QTime(0,0).addMSecs(duracionMS).toString("hh:mm:ss");
        cancionesWidgets[indice].txtDuracion->setText(duracion);
        player->stop();
        player->deleteLater();
        audioOutput->deleteLater();

    });
    player->play();
}

// Compatibilidad: si alguien llama al nombre viejo, redirigimos al global
void MenuAdmin::SeleccionarImagen()
{

    seleccionarImagenGlobal();

}

void MenuAdmin::subirCanciones()
{

    GestorCanciones gestor;
    Tipo tipoEnum=stringToTipo(comboTipo->currentText());
    const QString tipoStr =tipoToString(tipoEnum);

    //1) Validacion global
    const QString nombrePub =txtNombrePublicacion?txtNombrePublicacion->text().trimmed():"";
    const QString descGlobal= txtDescripcionGlobal?txtDescripcionGlobal->toPlainText().trimmed():QString();

    if(tipoEnum != Tipo::Single && nombrePub.isEmpty())
    {

        QMessageBox::warning(this, "Falta nombre", "Debes ingresar el nombre de la publicación (EP/Álbum).");
        return;

    }
    if(!txtRutaImagenGlobal || txtRutaImagenGlobal->text().isEmpty())
    {

        QMessageBox::warning(this, "Falta portada", "Selecciona una portada para la publicación.");
        return;

    }

    // 2) Cantidad esperada
    int esperadas= 1;
    if(tipoEnum== Tipo::EP) esperadas=3;
    else if(tipoEnum ==Tipo::Album)esperadas=8;

    if(cancionesWidgets.size()!=esperadas)
    {

        QMessageBox::warning(this, "Cantidad de pestañas", "La cantidad de pestañas no coincide con el tipo seleccionado.");
        return;

    }

    // 3) Ensamblar canciones
    int IDInicial=gestor.generarId();
    QVector<Cancion> aGuardar;
    QSet<QString> titulosUnicos, rutasAudioUnicas;

    for (int i=0; i<cancionesWidgets.size();++i)
    {

        const auto& w = cancionesWidgets[i];

        const QString titulo= w.txtTitulo->text().trimmed();
        const QString duracion= w.txtDuracion->text().trimmed();
        const QString rutaAudio= w.txtRutaAudio->text().trimmed();
        const QString rutaImagen= txtRutaImagenGlobal->text().trimmed();

        if(titulo.isEmpty() ||duracion.isEmpty()||rutaAudio.isEmpty()||rutaImagen.isEmpty())
        {

            QMessageBox::warning(this, "Campos incompletos",QString("Faltan datos en la canción %1. Título, audio, duración y portada son obligatorios.").arg(i+1));
            return;

        }

        if(titulosUnicos.contains(titulo))
        {

            QMessageBox::warning(this, "Título duplicado",QString("El título '%1' está repetido dentro del formulario.").arg(titulo));
            return;

        }
        if(rutasAudioUnicas.contains(rutaAudio))
        {

            QMessageBox::warning(this, "Audio duplicado",QString("La ruta de audio '%1' está repetida dentro del formulario.").arg(rutaAudio));
            return;

        }
        titulosUnicos.insert(titulo);
        rutasAudioUnicas.insert(rutaAudio);

        Genero g=stringToGenero(comboGeneroGlobal->currentText());
        Categoria c=stringToCategoria(comboCategoriaGlobal->currentText());

        //Descripcion
        QString descripcion;
        if(tipoEnum==Tipo::Single)
        {

            descripcion=QString();

        }else{

            descripcion=QString("[%1: %2]").arg(tipoStr, nombrePub);
            if (!descGlobal.isEmpty())
                descripcion+="\n"+descGlobal;

        }

        const int id=IDInicial+i;
        Cancion cancion(0,id, titulo, artista.getNombreArtistico(),g, c, tipoEnum, descripcion,rutaAudio, rutaImagen, duracion,QDate::currentDate(), true);

        if(gestor.CancionDuplicada(cancion))
        {

            QMessageBox::warning(this, "Canción duplicada",QString("La canción %1 no se puede guardar porque ya existe una con el mismo título, audio o imagen.").arg(i+1));
            return;

        }
        aGuardar.append(cancion);
    }

    // 4) Guardar
    int ok=0;
    for (const Cancion&c:aGuardar)
    {

        if(gestor.guardarCancion(c))++ok;

    }

    if(ok==aGuardar.size())
    {

        QMessageBox::information(this, "Éxito", "¡Todas las canciones se guardaron correctamente!");
        tipoSeleccionado(comboTipo->currentText()); // reinicia pestañas

    }
}

void MenuAdmin::LimpiarPanelDerecho()
{

    while(QLayoutItem*item =layoutDerecho->takeAt(0))
    {

        if(QWidget*widget= item->widget())
        {

            widget->deleteLater();

        }

        delete item;
    }
}

void MenuAdmin::MostrarPanelMiMusica()
{

    LimpiarPanelDerecho();
    // ELIMINAR panel anterior si existía
    if(panelReproductorAdmin){ panelReproductorAdmin->deleteLater(); panelReproductorAdmin=nullptr; }
    if(controlAdmin){ controlAdmin->deleteLater(); controlAdmin=nullptr; }

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

    lblCaratula=new QLabel; lblCaratula->setFixedSize(200,200);
    lblCaratula->setStyleSheet("border: 2px solid white; background-color: #333;");
    layoutTop->addWidget(lblCaratula);

    QVBoxLayout*layoutInfo=new QVBoxLayout;
    lblTitulo =new QLabel("Título");  lblTitulo->setStyleSheet("color: white; font-size: 18px;");
    lblArtista= new QLabel("Artista");lblArtista->setStyleSheet("color: gray; font-size: 14px;");
    lblTipo = new QLabel("Tipo");     lblTipo->setStyleSheet("color: gray;");
    lblReproducciones = new QLabel("Reproducciones: 0"); lblReproducciones->setStyleSheet("color: gray;");

    lblDescripcion=new QLabel("Descripcion: "); lblDescripcion->setStyleSheet("color: gray;"); lblDescripcion->setWordWrap(true);
    lblDescripcion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    lblGenero=new QLabel("Genero: "); lblGenero->setStyleSheet("color: gray;"); lblGenero->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    lblFechaCarga=new QLabel("Fecha Carga: "); lblFechaCarga->setStyleSheet("color: gray;"); lblFechaCarga->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    layoutInfo->addWidget(lblTitulo);
    layoutInfo->addWidget(lblArtista);
    layoutInfo->addWidget(lblTipo);
    layoutInfo->addWidget(lblReproducciones);
    layoutInfo->addWidget(lblDescripcion);
    layoutInfo->addWidget(lblGenero);
    layoutInfo->addWidget(lblFechaCarga);

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
    lblTiempoActual=new QLabel("00:00"); lblTiempoActual->setStyleSheet("color: white;");
    lblDuracionTotal=new QLabel("00:00"); lblDuracionTotal->setStyleSheet("color: white;");
    layoutTiempos->addWidget(lblTiempoActual);
    layoutTiempos->addStretch();
    layoutTiempos->addWidget(lblDuracionTotal);
    layout->addLayout(layoutTiempos);

    // ========== Controles ==========
    QHBoxLayout *layoutControles = new QHBoxLayout;

    btnAnterior=new QPushButton("⏮️");  btnAnterior->setToolTip("Pasar a la musica anterior");
    btnPlayPause=new QPushButton("▶️"); btnPlayPause->setToolTip("Play Musica");
    btnSiguiente= new QPushButton("⏭️");btnSiguiente->setToolTip("Pasar a la musica siguiente");
    btnRepetir =new QPushButton("🔁");  btnRepetir->setToolTip("Repetir Cancion infinitamente");
    btnAleatorio= new QPushButton("🔀"); btnAleatorio->setToolTip("Reproduccion Aleatoria");
    btnEditarCancionReproductor =new QPushButton("✏️"); btnEditarCancionReproductor->setToolTip("Editar Cancion");
    btnEliminarCancionReproductor= new QPushButton("🗑️"); btnEliminarCancionReproductor->setToolTip("Eliminar Cancion");

    QList<QPushButton*>botones={btnAnterior, btnPlayPause, btnSiguiente,btnRepetir, btnAleatorio, btnEditarCancionReproductor, btnEliminarCancionReproductor };
    for (auto btn : botones)
    {

        btn->setFixedSize(50, 50);
        btn->setStyleSheet("background-color: #222; color: white; border-radius: 25px;");
        layoutControles->addWidget(btn);

    }
    layout->addLayout(layoutControles);

    // ========== Lista de canciones ==========
    listaWidget= new QListWidget;
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

    //Seleccion inicial
    if(!listaCanciones.isEmpty())
    {

        listaWidget->setCurrentRow(0);
        mostrarDatosCancionActual(0);

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
        if(rep->playbackState()== QMediaPlayer::PlayingState)
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

        if(controlAdmin && controlAdmin->getReproductor())
        {

            qint64 nuevaPos=barraProgreso->value();
            controlAdmin->getReproductor()->setPosition(nuevaPos);

        }

    });

    // BOTONES navegación
    connect(btnSiguiente, &QPushButton::clicked, this, [=]()
    {

        controlAdmin->siguiente();
        btnPlayPause->setText("⏸️");

    });

    connect(btnAnterior, &QPushButton::clicked, this, [=]()
    {

        controlAdmin->anterior();
        btnPlayPause->setText("⏸️");

    });

    // BOTON REPETIR
    connect(btnRepetir, &QPushButton::clicked, this, [=]()
    {

        static bool repetirActivo=false;
        static bool aleatorioActivo=false;
        repetirActivo=!repetirActivo;

        if(repetirActivo){
            aleatorioActivo=false;
            controlAdmin->activarAleatorio(false);
            btnAleatorio->setStyleSheet("background-color: #222; color: white; border-radius: 25px;");
        }

        controlAdmin->activarRepetir(repetirActivo);
        btnRepetir->setStyleSheet(repetirActivo ?
                                      "background-color: #1DB954; color: black; border-radius: 25px;" :
                                      "background-color: #222; color: white; border-radius: 25px;");
    });

    // BOTON ALEATORIO
    connect(btnAleatorio, &QPushButton::clicked, this, [=]()
    {

        static bool repetirActivo=false;
        static bool aleatorioActivo=false;
        aleatorioActivo=!aleatorioActivo;

        if(aleatorioActivo)
        {

            repetirActivo=false;
            controlAdmin->activarRepetir(false);
            btnRepetir->setStyleSheet("background-color: #222; color: white; border-radius: 25px;");

        }
        controlAdmin->activarAleatorio(aleatorioActivo);
        btnAleatorio->setStyleSheet(aleatorioActivo ?
                                        "background-color: #1DB954; color: black; border-radius: 25px;" :
                                        "background-color: #222; color: white; border-radius: 25px;");
    });

    // BOTON ELIMINAR (admin)
    connect(btnEliminarCancionReproductor,&QPushButton::clicked,this,[=]()
    {

        if(listaCanciones.isEmpty())
        {

            QMessageBox::information(this,"Eliminar","No hay canciones para eliminar.");
            return;

        }

        bool ok=false;
        QString titulo=QInputDialog::getText(this,"Eliminar Cancion","Escribe el titulo EXACTO de la cancion a eliminar:",QLineEdit::Normal,"",&ok).trimmed();
        if(!ok||titulo.isEmpty())return;

        if(QMessageBox::question(this,"Confirmar",QString("¿Eliminar \"%1\"? Esta accion no se puede deshacer.").arg(titulo),QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes)
        {

            return;

        }

        GestorCanciones gestor;
        bool exito=gestor.eliminarCancionPorTituloYArtista(titulo,artista.getNombreArtistico());
        if(!exito){
            QMessageBox::warning(this,"eliminar","No se encontro ese titulo en tus canciones");
            return;
        }

        int idBorrar=-1;
        for(int i=0;i<listaCanciones.size();++i)
        {

            if(listaCanciones[i].getTitulo().compare(titulo,Qt::CaseInsensitive)==0)
            {

                idBorrar=i;break;

            }
        }
        if(idBorrar!=-1)
        {

            if(listaWidget->currentRow()==idBorrar)
            {

                controlAdmin->detener();
                lblTitulo->setText("Título");
                lblArtista->setText("Artista");
                lblTipo->setText("Tipo");
                lblReproducciones->setText("Reproducciones: 0");
                lblDuracionTotal->setText("00:00");
                lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation));
            }
            listaCanciones.removeAt(idBorrar);
            delete listaWidget->takeItem(idBorrar);

            controlAdmin->setListaCanciones(listaCanciones);

            if(!listaCanciones.isEmpty())
            {

                int nuevo=qBound(0,idBorrar,listaCanciones.size()-1);
                listaWidget->setCurrentRow(nuevo);

            }
            QMessageBox::information(this, "Eliminar", "Canción eliminada correctamente.");
        }
    });

    // BOTON EDITAR (admin)
    connect(btnEditarCancionReproductor,&QPushButton::clicked,this,[=]()
    {

        int idx=listaWidget->currentRow();
        if(idx<0||idx>=listaCanciones.size())
        {

            QMessageBox::information(this,"Editar","Selecciona una cancion primero.");
            return;

        }

        Cancion c=listaCanciones[idx];

        // DIALOGO
        QDialog dlg(this);
        dlg.setWindowTitle("Editar Cancion");
        QVBoxLayout*v=new QVBoxLayout(&dlg);

        QLabel*l1=new QLabel("Título:");
        QLineEdit*eTitulo=new QLineEdit(c.getTitulo());
        v->addWidget(l1); v->addWidget(eTitulo);

        QLabel*l2=new QLabel("Descripcion:");
        QTextEdit*eDesc=new QTextEdit(c.getDescripcion());
        eDesc->setMinimumHeight(70);
        v->addWidget(l2);v->addWidget(eDesc);

        QLabel*l3=new QLabel("Genero:");
        QComboBox*cbGenero=new QComboBox;
        cbGenero->addItems({"Pop","Corridos","Cristianos","Electronica","Regueton","Rock","Clasicas"});
        cbGenero->setCurrentText(generoToString(c.getGenero()));
        v->addWidget(l3); v->addWidget(cbGenero);

        QLabel*l4=new QLabel("Categoria:");
        QComboBox*cbCat=new QComboBox;
        cbCat->addItems({"playlist","recomendado","favorito","infantil","instrumental"});
        cbCat->setCurrentText(categoriaToString(c.getCategoria()));
        v->addWidget(l4); v->addWidget(cbCat);

        QLabel*l5=new QLabel("Carátula:");
        QHBoxLayout*hbCar =new QHBoxLayout;
        QLineEdit* eCaratula= new QLineEdit(c.getRutaImagen());
        eCaratula->setReadOnly(true);
        QPushButton* btnElegirCar= new QPushButton("Cambiar...");
        hbCar->addWidget(eCaratula,1);
        hbCar->addWidget(btnElegirCar);
        v->addWidget(l5); v->addLayout(hbCar);

        QLabel*prev=new QLabel;
        prev->setFixedSize(140, 140);
        prev->setStyleSheet("border:1px solid #444;");
        prev->setAlignment(Qt::AlignCenter);
        prev->setPixmap(QPixmap(QFile::exists(c.getRutaImagen())?c.getRutaImagen():":/imagenes/default_caratula.jpg").scaled(prev->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        v->addWidget(prev,0,Qt::AlignLeft);

        QObject::connect(btnElegirCar,&QPushButton::clicked,&dlg,[&]()
        {

            QString ruta=QFileDialog::getOpenFileName(&dlg,"Seleccionar carátula","","Imágenes (*.jpg *.jpeg *.png)");
            if(ruta.isEmpty())return;
            eCaratula->setText(ruta);
            prev->setPixmap(QPixmap(ruta).scaled(prev->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));

        });

        QHBoxLayout*hb=new QHBoxLayout;
        QPushButton*btnOk=new QPushButton("Guardar");
        QPushButton*btnCancel=new QPushButton("Cancelar");
        hb->addStretch();hb->addWidget(btnOk);hb->addWidget(btnCancel);
        v->addLayout(hb);

        QObject::connect(btnCancel, &QPushButton::clicked,&dlg,&QDialog::reject);

        QObject::connect(btnOk,&QPushButton::clicked,[&]()
        {

            QString nuevoTitulo=eTitulo->text().trimmed();
            QString nuevaDesc=eDesc->toPlainText().trimmed();
            QString nuevaCar=eCaratula->text().trimmed();

            if(nuevoTitulo.isEmpty()){
                QMessageBox::warning(&dlg,"Validacion","El titulo no puede estar vacio.");
                return;
            }
            if(nuevaCar.isEmpty()||!QFile::exists(nuevaCar)){
                QMessageBox::warning(&dlg, "Validacion", "Selecciona una caratula válida.");
                return;
            }

            GestorCanciones gestor;
            if(gestor.existeTituloORutaImagen(nuevoTitulo,nuevaCar,c.getId())){
                QMessageBox::warning(&dlg, "Duplicado","Ya existe una cancion en el sistema con ese TITULO o esa CARATULA.");
                return;
            }

            c.setTitulo(nuevoTitulo);
            c.setDescripcion(nuevaDesc);
            c.setGenero(stringToGenero(cbGenero->currentText()));
            c.setCategoria(stringToCategoria(cbCat->currentText()));
            c.setCaratula(nuevaCar);

            if(!gestor.ActualizarCancionPorId(c))
            {

                QMessageBox::critical(&dlg, "Error", "No se pudo guardar los cambios.");
                return;

            }

            listaCanciones[idx]=c;
            if(QListWidgetItem*it=listaWidget->item(idx))
            {

                it->setText(c.getTitulo());

            }

            lblTitulo->setText(c.getTitulo());
            lblTipo->setText(tipoToString(c.getTipo()));
            lblArtista->setText(c.getNombreArtista());
            lblDescripcion->setText("Descripcion: "+c.getDescripcion());
            if(QFile::exists(c.getRutaImagen()))
            {

                lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation));

            }
            QMessageBox::information(&dlg, "Editar", "Cambios guardados.");
            dlg.accept();
        });

        dlg.exec();
    });
}

QString MenuAdmin::formatearTiempo(qint64 milisegundos)
{

    int segundos=milisegundos/1000;
    int minutos=segundos/60;
    segundos%=60;
    return QString("%1:%2").arg(minutos,2,10,QChar('0')).arg(segundos,2,10,QChar('0'));

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

        const Cancion& c=listaCanciones[index];

        lblTitulo->setText(c.getTitulo());
        lblArtista->setText(c.getNombreArtista());
        lblTipo->setText(tipoToString(c.getTipo()));
        lblReproducciones->setText("Reproducciones: "+QString::number(c.getReproducciones()));
        lblDuracionTotal->setText(c.getDuracion().isEmpty()?"00:00":c.getDuracion());
        lblDescripcion->setText("Descripcion:"+c.getDescripcion());
        lblGenero->setText("Genero: "+generoToString(c.getGenero()));
        lblFechaCarga->setText("Fecha Carga: "+c.getFechaCarga().toString("dd/MM/yyyy"));

        if(QFile::exists(c.getRutaImagen()))
        {

            lblCaratula->setPixmap(QPixmap(c.getRutaImagen()).scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        }else{

            lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        }
        listaWidget->setCurrentRow(index);
    }
}

void MenuAdmin::ApagarReproductor()
{
    if(controlAdmin)
    {

        if(auto rep=controlAdmin->getReproductor())
        {

            rep->stop();
            QObject::disconnect(rep,nullptr,this,nullptr);

        }

    }
    LimpiarPanelDerecho();

    if (listaWidget) listaWidget->clear();
    listaCanciones.clear();

    if(controlAdmin){controlAdmin->deleteLater();controlAdmin=nullptr;}
    if(panelReproductorAdmin){panelReproductorAdmin->deleteLater();panelReproductorAdmin=nullptr;}

    if(lblTitulo) lblTitulo->setText("Título");
    if(lblArtista) lblArtista->setText("Artista");
    if(lblTipo) lblTipo->setText("Tipo");
    if(lblReproducciones) lblReproducciones->setText("Reproducciones: 0");
    if(lblDescripcion) lblDescripcion->setText("");
    if(lblDuracionTotal) lblDuracionTotal->setText("00:00");
    if(lblCaratula) lblCaratula->setPixmap(QPixmap(":/imagenes/default_caratula.jpg").scaled(200,200,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void MenuAdmin::MostrarPerfil()
{

    ApagarReproductor();

    QWidget*perfil=new QWidget;
    QVBoxLayout*root=new QVBoxLayout(perfil);
    root->setContentsMargins(24,24,24,24);
    root->setSpacing(18);

    QLabel*header=new QLabel("Mi Perfil de Artista");
    header->setStyleSheet("color:white;font-size:22px;font-weight:700;");
    root->addWidget(header);

    QHBoxLayout*top=new QHBoxLayout;
    top->setSpacing(20);

    QLabel*foto=new QLabel;
    foto->setFixedSize(140,140);
    foto->setStyleSheet("border-radius:70px; border:3px solid #1DB954; background:#222;");

    QPixmap px(artista.getRutaImagen());
    if(!px.isNull())
    {

        int lado=qMin(px.width(),px.height());
        QRect centro((px.width()-lado)/2,(px.height()-lado)/2,lado,lado);
        QPixmap cuadrado=px.copy(centro).scaled(foto->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

        QPixmap circular(foto->size());
        circular.fill(Qt::transparent);
        QPainter p(&circular);
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath path; path.addEllipse(0,0,foto->width(),foto->height());
        p.setClipPath(path);
        p.drawPixmap(0,0,cuadrado);
        p.end();
        foto->setPixmap(circular);

    }
    top->addWidget(foto,0,Qt::AlignTop);

    QVBoxLayout*rightTop=new QVBoxLayout;
    QLabel*lblNombreArt=new QLabel(artista.getNombreArtistico());
    lblNombreArt->setStyleSheet("color:white; font-size:28px; font-weight:800;");
    rightTop->addWidget(lblNombreArt);

    QGridLayout*grid=new QGridLayout;
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(8);

    auto mKLabel=[](const QString&k,const QString&v)
    {

        QWidget*w=new QWidget;
        QHBoxLayout*h=new QHBoxLayout(w);
        h->setContentsMargins(0,0,0,0);
        QLabel*lk=new QLabel(k+":");
        lk->setStyleSheet("color:#b3b3b3; font-size:14px;");
        QLabel*lv=new QLabel(v);
        lv->setStyleSheet("color:white;font-size:14px");
        h->addWidget(lk);
        h->addWidget(lv,1);
        return w;

    };

    grid->addWidget(mKLabel("ID",QString::number(artista.getId())),0,0);
    grid->addWidget(mKLabel("Pais",artista.getPais()),0,1);
    grid->addWidget(mKLabel("Genero Musical",artista.getGenero()),1,0);
    grid->addWidget(mKLabel("Registro",artista.getFechaRegistro().toString("yyyy-MM-dd")),1,1);
    grid->addWidget(mKLabel("Nacimiento",artista.getFechaNacimiento().toString("yyyy-MM-dd")),2,0);

    rightTop->addLayout(grid);
    top->addLayout(rightTop,1);

    root->addLayout(top);

    QLabel*bioTitle=new QLabel("Biografia");
    bioTitle->setStyleSheet("color:white; font-size:16px; font-weight:600;");
    root->addWidget(bioTitle);

    QTextEdit*bio=new QTextEdit;
    bio->setReadOnly(true);
    bio->setText(artista.getBiografia());
    bio->setMinimumHeight(140);
    bio->setStyleSheet("QTextEdit { background:#1a1a1a; color:#ddd; border:1px solid #333; border-radius:8px; padding:10px; }");

    root->addWidget(bio);

    layoutDerecho->addWidget(perfil);
}

QWidget* MenuAdmin::crearTarjetaSeccion(const QString &titulo, QWidget *contenido)
{
    auto*card=new QFrame;
    card->setStyleSheet(
        "QFrame{background-color:#1a1a1a;border:1px solid #333;border-radius:12px;}"
        "QLabel[role='title']{color:white;font-size:18px;font-weight:700;}"
        );

    auto* v = new QVBoxLayout(card);
    v->setContentsMargins(16,16,16,16);
    v->setSpacing(10);

    auto*lbl=new QLabel(titulo);
    lbl->setProperty("role","title");

    contenido->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    v->addWidget(lbl, 0, Qt::AlignLeft);
    v->addWidget(contenido, 1);

    return card;
}

QTableWidget* MenuAdmin::tablaDummy(const QStringList &cabeceras, int filas,bool conRanking)
{
    QStringList headers=cabeceras;
    if(conRanking)
    {

        if(headers.isEmpty()||headers.first()!="#")
            headers.insert(0,"#");

    }

    auto*tabla=new QTableWidget(filas, headers.size());
    tabla->setItemDelegate(new CenterDelegate(tabla));

    //ENCABEZADOS
    tabla->setHorizontalHeaderLabels(headers);
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->horizontalHeader()->setFixedHeight(36);

    //TABLA
    tabla->verticalHeader()->setVisible(false);
    tabla->setShowGrid(false);
    tabla->setSelectionMode(QAbstractItemView::NoSelection);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setAlternatingRowColors(true);
    tabla->setWordWrap(false);
    tabla->setFocusPolicy(Qt::NoFocus);
    tabla->setFrameShape(QFrame::NoFrame);
    tabla->setContentsMargins(6,6,6,6);

    //ALTURAS
    const int rowH=38;
    tabla->verticalHeader()->setDefaultSectionSize(rowH);
    const int headerH=36;
    const int wanted=headerH+filas*rowH+24;
    const int minH=qMin(wanted,420);
    tabla->setMinimumHeight(minH);
    tabla->setMaximumHeight(minH);
    tabla->setStyleSheet(
        "QHeaderView::section{"
        "  background:#1c1c1c; color:#cfcfcf; border:none; padding:8px 10px;"
        "  font-weight:600; border-top-left-radius:8px; border-top-right-radius:8px;"
        "}"
        "QTableWidget{"
        "  background:#111; color:#e6e6e6; border:1px solid #2b2b2b; border-radius:12px;"
        "  padding:6px;"
        "}"
        "QTableCornerButton::section{ background:#1c1c1c; border:none; }"
        "QTableWidget::item{ padding:10px; }"
        "QTableWidget::item:alternate{ background:#141414; }"
        );

    for(int r=0;r<filas;++r)
    {

        int c0=0;
        if(conRanking)
        {

            auto* posItem=new QTableWidgetItem(QString::number(r+1));
            posItem->setTextAlignment(Qt::AlignCenter);
            posItem->setFlags(Qt::ItemIsEnabled);
            tabla->setItem(r, c0++,posItem);

        }
        for(int c=c0;c<headers.size();++c)
        {

            auto*it=new QTableWidgetItem(QStringLiteral("—"));
            it->setTextAlignment(Qt::AlignCenter);
            it->setFlags(Qt::ItemIsEnabled);
            tabla->setItem(r,c,it);

        }
    }

    //COLUMNA RANKING ESTRECHA
    if(conRanking)
    {

        tabla->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
        tabla->setColumnWidth(0,56);
        for(int c =1;c<headers.size();++c)
            tabla->horizontalHeader()->setSectionResizeMode(c,QHeaderView::Stretch);

    }
    return tabla;
}
/*

    aqui uso una tabla hash porque es la forma mas simple y rapida de contar y agrupar cosas por ID (canciones,usuarios)

    uso hash tambien porque es mas rapida con notacion O(1),no mantiene un orden

    en pocas palabras es mi contador por id rapido, luego de eso se ordena el resultado para mostrar rankings

*/
void MenuAdmin::MostrarEstadisticas()
{

    ApagarReproductor();
    LimpiarPanelDerecho();

    // ====== Preparacion de datos (leer eventos y catlogos) ======
    GestorReproduccion gr("reproducciones.dat");

    QVector<EventoReproduccion>evs;
    gr.leerTodos(evs);

    GestorCanciones gc;
    const QVector<Cancion>todas=gc.leerCanciones();

    // Mapas de apoyo
    QHash<quint32,Cancion>songById;
    songById.reserve(todas.size());

    QSet<quint32>idsDeEsteArtista;
    const QString artistaRef=artista.getNombreArtistico().trimmed().toLower();

    for(const auto&c:todas)
    {

        const quint32 sid=static_cast<quint32>(c.getId());
        songById.insert(sid,c);
        if(c.getNombreArtista().trimmed().toLower()==artistaRef)
            idsDeEsteArtista.insert(sid);

    }

    // Contadores
    QHash<quint32,int>cntGlobalCancion;   // songId -> escuchas validas (global)
    QHash<quint32,int>cntArtistaCancion;  // songId -> escuchas validas (solo canciones del artista)
    QHash<quint32,int>cntUsuario;         // userId  -> actividad (eventos)
    QVector<qint64>sumMsPorDia(7,0);     // 0..6 = Lun..Dom

    qint64 minEpoch=std::numeric_limits<qint64>::max();
    qint64 maxEpoch=0;

    for(const auto&ev:evs)
    {

        if(!songById.contains(ev.songId))continue;

        // Rango temporal y sumas por dia
        minEpoch=qMin(minEpoch, static_cast<qint64>(ev.epochMs));
        maxEpoch=qMax(maxEpoch, static_cast<qint64>(ev.epochMs));
        const int dow=QDateTime::fromMSecsSinceEpoch(ev.epochMs).date().dayOfWeek(); // 1..7 (Lun..Dom)
        sumMsPorDia[dow-1]+=ev.msPlayed;

        // Actividad de usuario (eventos)
        const bool valida=gr.cuentaComoEscucha(ev.msPlayed,ev.durMs);
        if(valida)
        {

            cntUsuario[ev.userId]+=1;

            //rankings por cancion (global y del artista)
            cntGlobalCancion[ev.songId]+=1;
            if(idsDeEsteArtista.contains(ev.songId))
                cntArtistaCancion[ev.songId]+=1;

        }


    }

    // Promedios por dia (semanal): total ms / #semanas en el rango
    double semanas=1.0;
    if(maxEpoch > minEpoch)
    {

        const double dias=(maxEpoch-minEpoch)/1000.0/60.0/60.0/24.0;
        semanas=qMax(1.0,dias/7.0);

    }
    QVector<qint64> promMsPorDia(7,0);
    qint64 maxAvg=0;
    for(int i=0;i<7;++i)
    {

        promMsPorDia[i]=static_cast<qint64>(sumMsPorDia[i]/semanas);
        maxAvg=qMax(maxAvg,promMsPorDia[i]);

    }
    auto percent=[&](qint64 v)->int
    {


        return (maxAvg>0)?int((v*100)/maxAvg):0;
    };

    // Helper para ordenar hash -> vector (desc por conteo)
    auto ordenarTop=[](const QHash<quint32,int>& h,int topN)
    {

        QVector<QPair<quint32,int>> v; v.reserve(h.size());
        for (auto it=h.begin();it!=h.end();++it)v.push_back({it.key(),it.value()});
        std::sort(v.begin(),v.end(), [](auto a, auto b){ return a.second > b.second; });
        if (v.size() > topN) v.resize(topN);
        return v;

    };

    const auto top10Global= ordenarTop(cntGlobalCancion, 10);
    const auto topArtista= ordenarTop(cntArtistaCancion, 8);
    const auto topUsuarios=ordenarTop(cntUsuario, 8);

    auto*scroll= new QScrollArea; scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea{border:none;}");

    auto*wrap=new QWidget;
    auto*root=new QVBoxLayout(wrap);
    root->setContentsMargins(22,22,22,22);
    root->setSpacing(18);

    auto *titulo= new QLabel("Estadísticas");
    titulo->setStyleSheet("color:white;font-size:24px;font-weight:800;");
    root->addWidget(titulo);

    // --- Top 10 global ---
    auto *tblTop10= tablaDummy({"#", "Título", "Artista", "Reproducciones"}, 10, true);
    root->addWidget(crearTarjetaSeccion("Top 10 canciones más escuchadas", tblTop10));

    // --- Ranking de tus canciones (global) ---
    auto *tblPorArtista = tablaDummy({"#", "Título", "Reproducciones"}, 8, true);
    root->addWidget(crearTarjetaSeccion(QString("Ranking global de tus canciones (%1)").arg(artista.getNombreArtistico()),tblPorArtista));

    const int TOPN_CAL=8;  // filas visibles
    const int MIN_VOTOS=1;  // umbral minimo de votos

    auto* tblMejorCal = tablaDummy({"Título", "Artista", "★ Promedio", "Votos"}, TOPN_CAL, true);
    root->addWidget(crearTarjetaSeccion("Canciones mejor calificadas (promedio)", tblMejorCal));

    // === Datos desde calificaciones.dat
    GestorCalificaciones gcal;
    const auto topRate = gcal.topCancionesMejorCalif(TOPN_CAL, MIN_VOTOS);

    // OJOOOOOOOOOO: aqui REUSAMOS songById que ya fue construido arriba con todas las canciones vigentes

    tblMejorCal->clearContents();
    tblMejorCal->setRowCount(TOPN_CAL);

    // Rellenar filas reales (solo canciones que siguen en canciones.dat)
    int r=0;
    for(int i=0;i<topRate.size()&&r<TOPN_CAL;++i)
    {

        const quint32 songId=topRate[i].first;
        const double  avg=topRate[i].second;

        //Filtra: si la cancion ya no existe en el catalogo vigente, se omite (sin rastro)
        if(!songById.contains(songId))
            continue;

        const Cancion&c=songById[songId];

        // Votos (conteo actual)
        double dummy=0.0;
        int votos=0;
        gcal.promedioCancion(songId, dummy, votos);

        // # Puesto (col 0)
        auto rankIt = new QTableWidgetItem(QString::number(r+1));
        rankIt->setFlags(Qt::ItemIsEnabled);
        rankIt->setTextAlignment(Qt::AlignCenter);
        tblMejorCal->setItem(r,0,rankIt);

        // Título (col 1)
        auto tIt = new QTableWidgetItem(c.getTitulo());
        tIt->setFlags(Qt::ItemIsEnabled);
        tIt->setTextAlignment(Qt::AlignCenter);
        tblMejorCal->setItem(r,1,tIt);

        // Artista (col 2)
        auto aIt=new QTableWidgetItem(c.getNombreArtista());
        aIt->setFlags(Qt::ItemIsEnabled);
        aIt->setTextAlignment(Qt::AlignCenter);
        tblMejorCal->setItem(r,2,aIt);

        // ★ Promedio (col 3)
        auto pIt = new QTableWidgetItem(QString::number(avg, 'f', 2));
        pIt->setFlags(Qt::ItemIsEnabled);
        pIt->setTextAlignment(Qt::AlignCenter);
        tblMejorCal->setItem(r,3,pIt);

        // Votos (col 4)
        auto vIt=new QTableWidgetItem(QString::number(votos));
        vIt->setFlags(Qt::ItemIsEnabled);
        vIt->setTextAlignment(Qt::AlignCenter);
        tblMejorCal->setItem(r,4,vIt);

        ++r;
    }

    //Completar filas vacias con "—"
    for(; r<TOPN_CAL; ++r)
    {

        auto rankIt = new QTableWidgetItem(QString::number(r + 1));
        rankIt->setFlags(Qt::ItemIsEnabled);
        rankIt->setTextAlignment(Qt::AlignCenter);
        tblMejorCal->setItem(r, 0, rankIt);

        for(int c=1;c<=4;++c)
        {

            auto dash= new QTableWidgetItem(QStringLiteral("—"));
            dash->setFlags(Qt::ItemIsEnabled);
            dash->setTextAlignment(Qt::AlignCenter);
            tblMejorCal->setItem(r,c,dash);
        }

    }

    // --- Promedio de calificacion de tus canciones (artista actual) ---
    {

        const int TOPN_USR_CAL=8;

        QTableWidget* tblCalArt=tablaDummy({"Título", "★ Promedio", "Votos"}, TOPN_USR_CAL, /*conRanking*/true);
        root->addWidget(crearTarjetaSeccion(QString("Promedio de calificación de tus canciones (%1)").arg(artista.getNombreArtistico()),tblCalArt));

        GestorCalificaciones gcal2;

        struct Row{quint32 sid; double avg; int votos;};
        QVector<Row> rows;rows.reserve(idsDeEsteArtista.size());

        //Solo canciones de este artista
        for(auto it=idsDeEsteArtista.constBegin(); it!=idsDeEsteArtista.constEnd();++it)
        {

            double avg =0.0; int cnt=0;
            gcal2.promedioCancion(*it,avg,cnt);// promedio y votos de esa cancion
            if(cnt>0) rows.push_back({*it,avg,cnt});

        }

        // Orden: mejor promedio y, si empatan, mas votos primero
        std::sort(rows.begin(), rows.end(), [](const Row& a, const Row& b)
        {

            if(a.avg!=b.avg)return a.avg>b.avg;
            return a.votos>b.votos;

        });

        // Pintar tabla
        tblCalArt->clearContents();
        tblCalArt->setRowCount(TOPN_USR_CAL);

        int r=0;
        for(;r <rows.size() && r<TOPN_USR_CAL;++r)
        {

            const quint32 sid=rows[r].sid;
            const QString titulo= songById.contains(sid)?songById[sid].getTitulo():QString("ID %1").arg(sid);

            // # ranking
            auto rankIt=new QTableWidgetItem(QString::number(r+1));
            rankIt->setFlags(Qt::ItemIsEnabled);
            tblCalArt->setItem(r,0,rankIt);

            //Titulo
            auto tIt=new QTableWidgetItem(titulo);
            tIt->setFlags(Qt::ItemIsEnabled);
            tblCalArt->setItem(r,1,tIt);

            // ★ Promedio
            auto pIt=new QTableWidgetItem(QString::number(rows[r].avg,'f',2));
            pIt->setFlags(Qt::ItemIsEnabled);
            tblCalArt->setItem(r,2,pIt);

            // Votos
            auto vIt=new QTableWidgetItem(QString::number(rows[r].votos));
            vIt->setFlags(Qt::ItemIsEnabled);
            tblCalArt->setItem(r,3,vIt);

        }

        // Completar con guiones si sobran filas
        for(; r<TOPN_USR_CAL; ++r)
        {

            auto rankIt=new QTableWidgetItem(QString::number(r+1));
            rankIt->setFlags(Qt::ItemIsEnabled);
            tblCalArt->setItem(r,0,rankIt);

            for(int c= 1; c <=3; ++c)
            {

                auto dash =new QTableWidgetItem(QStringLiteral("—"));
                dash->setFlags(Qt::ItemIsEnabled);
                tblCalArt->setItem(r,c,dash);

            }

        }

    }

    // --- Promedio general de calificacion por genero ---
    {

        // Tabla de 7 filas (los 7 géneros) con columna "#"
        QTableWidget*tblGen=tablaDummy({"Género", "★ Promedio", "Votos"}, /*filas*/7, /*conRanking*/true);
        root->addWidget(crearTarjetaSeccion("Promedio general de calificación por género", tblGen));

        // 1) Cargar TODAS las calificaciones
        GestorCalificaciones gcal;
        QVector<RegistroCalificacion> allRates;
        gcal.leerTodas(allRates); // lee calificaciones.dat  (userId, songId, rating, epochMs)

        // 2) Acumular por genero usando el catálogo (songId -> Cancion -> Genero)
        //Nota: usamos 'int' como clave para evitar necesidad de qHash(Genero)
        QHash<int, QPair<long long,int>> acum; // generoIdx -> (sumaRatings, conteo)
        for(const auto& r:allRates)
        {

            if (!songById.contains(r.songId)) continue;
            const Genero g=songById[r.songId].getGenero();
            auto &p=acum[static_cast<int>(g)];
            p.first+=r.rating;
            p.second+=1;

        }

        // 3) Armamos filas: primero generos con datos (ordenados por promedio desc y, si empatan, por votos),
        //    luego los generos sin datos.
        struct Row { int gIdx; double avg; int votos; };
        QVector<Row> withData;
        QVector<int>  noData;

        const QVector<Genero> ordered =
        {

            Genero::Pop, Genero::Corridos, Genero::Cristianos,
            Genero::Electronica, Genero::Regueton, Genero::Rock, Genero::Clasicas

        };

        for(Genero g : ordered)
        {

            const int key=static_cast<int>(g);
            auto it=acum.find(key);
            if(it!=acum.end() && it.value().second>0)
            {

                const double avg=double(it.value().first)/double(it.value().second);
                withData.push_back({key,avg,it.value().second});

            }else{

                noData.push_back(key);

            }

        }

        std::sort(withData.begin(), withData.end(),[](const Row& a, const Row& b){
                      if (a.avg != b.avg) return a.avg > b.avg;  // mejor promedio primero
                      return a.votos > b.votos;                   // si empatan, más votos primero
                });

        // 4) Pintar tabla (7 filas): primero con datos y al final los que no tienen votos
        tblGen->clearContents();
        tblGen->setRowCount(7);

        int r=0;

        // Con datos
        for(; r< withData.size()&&r<7;++r)
        {

            const Row &row=withData[r];
            const QString gName =generoToString(static_cast<Genero>(row.gIdx));

            // Col 0 ya es "#": ponemos el ranking (r+1)
            auto rankIt=new QTableWidgetItem(QString::number(r+1));
            rankIt->setFlags(Qt::ItemIsEnabled);
            tblGen->setItem(r,0,rankIt);

            // Genero
            auto gIt=new QTableWidgetItem(gName); gIt->setFlags(Qt::ItemIsEnabled);
            tblGen->setItem(r,1,gIt);

            // Promedio
            auto pIt=new QTableWidgetItem(QString::number(row.avg,'f',2));
            pIt->setFlags(Qt::ItemIsEnabled);
            tblGen->setItem(r,2,pIt);

            // Votos
            auto vIt=new QTableWidgetItem(QString::number(row.votos));
            vIt->setFlags(Qt::ItemIsEnabled);
            tblGen->setItem(r,3,vIt);
        }

        // Sin datos (completamos hasta 7)
        for(int k = 0; r<7&& k<noData.size(); ++k, ++r)
        {

            const QString gName =generoToString(static_cast<Genero>(noData[k]));

            auto rankIt=new QTableWidgetItem(QString::number(r+1));
            rankIt->setFlags(Qt::ItemIsEnabled);
            tblGen->setItem(r,0,rankIt);

            auto gIt =new QTableWidgetItem(gName); gIt->setFlags(Qt::ItemIsEnabled);
            tblGen->setItem(r,1,gIt);

            for(int c=2; c<=3; ++c)
            {

                auto dash=new QTableWidgetItem(QStringLiteral("—"));
                dash->setFlags(Qt::ItemIsEnabled);
                tblGen->setItem(r,c,dash);

            }
        }

    }

    // --- Usuarios mas activos ---
    auto *tblUsuariosAct =tablaDummy({"#", "Usuario", "Reproducciones"}, 8, true);
    root->addWidget(crearTarjetaSeccion("Usuarios más activos por cantidad de reproducciones", tblUsuariosAct));

    // --- Tiempos promedio de uso por dia ---
    auto *boxTiempo=new QFrame;
    boxTiempo->setStyleSheet("QFrame{background:#111;border:1px solid #333;border-radius:10px;}");
    auto*vTiempo=new QVBoxLayout(boxTiempo);
    vTiempo->setContentsMargins(12,12,12,12);
    vTiempo->setSpacing(10);

    QStringList dias={"Lun","Mar","Mié","Jue","Vie","Sáb","Dom"};
    for(int i=0;i<dias.size();++i)
    {

        auto*fila=new QWidget; auto*h=new QHBoxLayout(fila);
        h->setContentsMargins(0,0,0,0); h->setSpacing(8);
        auto*lbl=new QLabel(dias[i]); lbl->setStyleSheet("color:#bbb;width:34px;");
        auto*bar=new QProgressBar; bar->setRange(0,100); bar->setValue(10+i*10);
        bar->setValue(percent(promMsPorDia[i]));
        bar->setTextVisible(false);
        bar->setStyleSheet(
            "QProgressBar{background:#1c1c1c;border:1px solid #333;border-radius:6px;height:12px;}"
            "QProgressBar::chunk{background:#4caf50;border-radius:6px;}"
            );
        h->addWidget(lbl);
        h->addWidget(bar,1);
        vTiempo->addWidget(fila);

    }
    root->addWidget(crearTarjetaSeccion("Tiempos promedio de uso del sistema (por día)", boxTiempo));

    // ====== Volcar datos a tablas =====
    // Top 10 global (Titulo — Artista, Reproducciones)
    tblTop10->setRowCount(top10Global.size());
    for(int i=0;i<top10Global.size();++i)
    {

        const auto sid=top10Global[i].first;
        const int n=top10Global[i].second;

        QString titulo=QString("ID %1").arg(sid);
        QString artistaCancion="—";
        if(songById.contains(sid))
        {

            const auto&c=songById[sid];
            titulo=c.getTitulo();
            artistaCancion=c.getNombreArtista();

        }

        tblTop10->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));


        auto*itTitulo=new QTableWidgetItem(titulo);
        auto*itArtista= new QTableWidgetItem(artistaCancion);
        auto*itRepros=new QTableWidgetItem(QString::number(n));

        itTitulo->setTextAlignment(Qt::AlignCenter);
        itArtista->setTextAlignment(Qt::AlignCenter);
        itRepros->setTextAlignment(Qt::AlignCenter);

        tblTop10->setItem(i,1,itTitulo);
        tblTop10->setItem(i,2,itArtista);
        tblTop10->setItem(i,3,itRepros);

    }

    // Ranking de tus canciones (sólo del artista actual)
    tblPorArtista->setRowCount(topArtista.size());
    for(int i=0;i<topArtista.size();++i)
    {

        const auto sid=topArtista[i].first;
        const int n=topArtista[i].second;

        QString titulo=songById.contains(sid)?songById[sid].getTitulo():QString("ID %1").arg(sid);
        tblPorArtista->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));


        auto*itTitulo =new QTableWidgetItem(titulo);
        auto*itRepros= new QTableWidgetItem(QString::number(n));

        itTitulo->setTextAlignment(Qt::AlignCenter);
        itRepros->setTextAlignment(Qt::AlignCenter);

        tblPorArtista->setItem(i,1,itTitulo);
        tblPorArtista->setItem(i,2,itRepros);

    }

    // Usuarios mas activos (id -> nombre)
    GestorUsuarios gu;
    const auto usuarios=gu.leerUsuarios();
    QHash<quint32, QString> nombreUsuario;
    for(const auto &u:usuarios)
        nombreUsuario.insert(static_cast<quint32>(u.getId()), u.getNombreUsuario());

    tblUsuariosAct->setRowCount(topUsuarios.size());
    for(int i=0;i<topUsuarios.size();++i)
    {

        const auto uid=topUsuarios[i].first;
        const int n=topUsuarios[i].second;

        const QString nombre=nombreUsuario.value(uid, QString("ID %1").arg(uid));

        tblUsuariosAct->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));

        auto*itNombre=new QTableWidgetItem(nombre);
        auto*itRepros=new QTableWidgetItem(QString::number(n));

        itNombre->setTextAlignment(Qt::AlignCenter);
        itRepros->setTextAlignment(Qt::AlignCenter);

        tblUsuariosAct->setItem(i,1,itNombre);
        tblUsuariosAct->setItem(i,2,itRepros);

    }

    // ====== Montaje final ======
    scroll->setWidget(wrap);
    layoutDerecho->addWidget(scroll);

}

void MenuAdmin::seleccionarImagenGlobal()
{

    QString ruta=QFileDialog::getOpenFileName(this, "Seleccionar portada", "", "Imágenes (*.jpg *.jpeg *.png)");
    if(ruta.isEmpty())return;

    txtRutaImagenGlobal->setText(ruta);
    QPixmap pix(ruta);
    if(!pix.isNull())
    {

        vistaPortadaGlobal->setPixmap(pix.scaled(vistaPortadaGlobal->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    }else{

        vistaPortadaGlobal->setPixmap(QPixmap());
        vistaPortadaGlobal->setText("Sin imagen");

    }

}

void MenuAdmin::seleccionarAudiosMultiples()
{
    int esperadas=1;
    const QString t=comboTipo->currentText();
    if(t=="EP") esperadas=3;
    else if(t=="Álbum") esperadas=8;

    QStringList rutas=QFileDialog::getOpenFileNames(this, "Seleccionar audios", "", "Audio (*.mp3 *.mp4 *.m4a)");
    if(rutas.isEmpty())return;

    if(rutas.size()!=esperadas)
    {

        QMessageBox::warning(this,"Cantidad incorrecta",QString("Debes seleccionar exactamente %1 archivos de audio para un %2.").arg(esperadas).arg(t));
        return;

    }

    //Rellenar y calcular duracion (silencioso)
    for(int i=0;i<rutas.size()&&i<cancionesWidgets.size();++i)
    {

        cancionesWidgets[i].txtRutaAudio->setText(rutas[i]);

        QMediaPlayer*player=new QMediaPlayer(this);
        QAudioOutput*out=new QAudioOutput(this);
        out->setVolume(0); //que no suene
        player->setAudioOutput(out);
        player->setSource(QUrl::fromLocalFile(rutas[i]));
        connect(player,&QMediaPlayer::durationChanged,this,[this,i,player,out](qint64 ms)
        {

            cancionesWidgets[i].txtDuracion->setText(QTime(0,0).addMSecs(ms).toString("hh:mm:ss"));
            player->stop();
            player->deleteLater();
            out->deleteLater();

        });
        player->play();
    }
}


