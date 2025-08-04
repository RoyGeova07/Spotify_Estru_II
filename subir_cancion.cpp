#include "subir_cancion.h"
#include<QMediaPlayer>
#include<QFileDialog>
#include<QTime>
#include <QUrl>
#include<QVBoxLayout>
#include<QMessageBox>
#include<QDate>
#include<QAudioOutput>
#include"gestorcanciones.h"
#include<QScrollArea>
#include"menuadmin.h"

Subir_Cancion::Subir_Cancion(const Artista& artistaActivo,QWidget*parent):QWidget(parent),artista(artistaActivo)
{

    configurarUI();

}

void Subir_Cancion::configurarUI()
{

    setWindowTitle("Subir Canciones");
    setFixedSize(700,730);

    QVBoxLayout*layoutPrincipal=new QVBoxLayout(this);

    btnVolver=new QPushButton("<-- Volver");
    btnVolver->setFixedSize(100,35);
    btnVolver->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 10px; font-size: 14px; }"
                             "QPushButton:hover { background-color: #388E3C; }");

    connect(btnVolver,&QPushButton::clicked,this,&Subir_Cancion::volverAlMenu);

    QHBoxLayout* layoutSuperior = new QHBoxLayout;
    layoutSuperior->addWidget(btnVolver);
    layoutSuperior->addStretch();

    layoutPrincipal->addLayout(layoutSuperior);

    lblNombreArtista=new QLabel("Artista: "+artista.getNombreArtistico());
    lblNombreArtista->setStyleSheet("font-weight: bold; font-size: 20px; color: #4CAF50;");
    lblNombreArtista->setAlignment(Qt::AlignCenter);

    QLabel*lblTipo =new QLabel("Tipo de publicación:");
    comboTipo=new QComboBox;
    comboTipo->addItems({"Single", "EP", "Álbum"});
    comboTipo->setCurrentIndex(0);
    connect(comboTipo, &QComboBox::currentTextChanged, this, &Subir_Cancion::tipoSeleccionado);

    //aqui los widgets de pestañas por cancion
    tabsCanciones=new QTabWidget;
    crearPestañasCanciones(1);//aqui single por defecto
    setStyleSheet(R"(
    QWidget {
        background-color: #202020;
        color: white;
        font-family: 'Segoe UI', sans-serif;
    }

    QLabel {
        font-weight: bold;
        font-size: 13px;
        margin-top: 6px;
        margin-bottom: 2px;
    }

    QLineEdit, QTextEdit, QComboBox {
        background-color: #2c2c2c;
        color: white;
        border: 1px solid #555;
        border-radius: 5px;
        padding: 4px;
        font-size: 13px;
    }

    QTextEdit {
        min-height: 30px;
        max-height: 40px;
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

    btnSubir =new QPushButton("🎵 Subir Canciones");
    btnSubir->setFixedHeight(40);
    btnSubir->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 10px; font-size: 16px; }"
                            "QPushButton:hover { background-color: #388E3C; }");

    connect(btnSubir, &QPushButton::clicked, this, &Subir_Cancion::subirCanciones);

    // Agregar al layout principal
    layoutPrincipal->addWidget(lblNombreArtista);
    layoutPrincipal->addSpacing(10);
    layoutPrincipal->addWidget(lblTipo);
    layoutPrincipal->addWidget(comboTipo);
    layoutPrincipal->addWidget(tabsCanciones);
    layoutPrincipal->addSpacing(10);
    layoutPrincipal->addWidget(btnSubir);


}

void Subir_Cancion::tipoSeleccionado(const QString &tipo)
{

    int cantidad=1;
    if(tipo=="EP")cantidad=3;
    else if(tipo=="Álbum")cantidad=8;

    crearPestañasCanciones(cantidad);

}

QWidget*Subir_Cancion::crearFormularioCancion(int /*index*/,int ID)
{

    QWidget*contenido=new QWidget;
    WidgetCancion w;

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

    connect(w.btnSeleccionarAudio,&QPushButton::clicked,this,&Subir_Cancion::seleccionarAudio);
    connect(w.btnSeleccionarImagen,&QPushButton::clicked,this,&Subir_Cancion::SeleccionarImagen);

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

void Subir_Cancion::crearPestañasCanciones(int cantidad)
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

void Subir_Cancion::seleccionarAudio()
{

    int indice=tabsCanciones->currentIndex();//pestania actual
    if(indice<0||indice>=cancionesWidgets.size())return;

    QString ruta=QFileDialog::getOpenFileName(this,"Seleccionar archivo de audio","","Audio (*.mp3 *.mp4)");
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

void Subir_Cancion::subirCanciones()
{

    GestorCanciones gestor;
    Tipo tipoEnum =stringToTipo(comboTipo->currentText());

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
        Cancion cancion(id, titulo, artista.getNombreArtistico(), g, c, tipoEnum, descripcion,rutaAudio, rutaImagen, duracion, QDate::currentDate(), true);

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

void Subir_Cancion::SeleccionarImagen()
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

void Subir_Cancion::volverAlMenu()
{

    MenuAdmin*m=new MenuAdmin(artista);
    m->show();
    this->close();

}
