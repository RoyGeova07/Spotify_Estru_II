#include "PantallaAdmin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include "gestorcanciones.h"
#include<QMediaPlayer>
#include<QMediaMetaData>

PantallaAdmin::PantallaAdmin(const Artista &artistaActivo, QWidget *parent): QWidget(parent), artista(artistaActivo)
{

    setWindowTitle("Panel del Administrador - Subir Canción");
    setFixedSize(600, 500);
    configurarUI();

}

void PantallaAdmin::configurarUI()
{

    QVBoxLayout*layoutPrincipal =new QVBoxLayout(this);

    //Etiquetas de audio y portada
    lblAudioSeleccionado =new QLabel("Archivo de audio: (No seleccionado)");
    lblPortadaSeleccionada= new QLabel("Portada: (No seleccionada)");
    lblVistaPreviaPortada=new QLabel();
    lblVistaPreviaPortada->setFixedSize(150,150);
    lblVistaPreviaPortada->setStyleSheet("border: 1px solid gray;");
    lblVistaPreviaPortada->setAlignment(Qt::AlignCenter);

    lblTitulo= new QLabel("Título: -");
    lblDuracion =new QLabel("Duración: -");

    btnSeleccionarAudio =new QPushButton("Seleccionar Archivo MP3/MP4");
    btnSeleccionarPortada= new QPushButton("Seleccionar Imagen de Portada");

    // ComboBoxes
    comboGenero =new QComboBox();
    comboGenero->addItems({"Pop", "Corridos", "Cristianos", "Electrónica", "Reguetón", "Rock", "Clásicas"});

    comboCategoria= new QComboBox();
    comboCategoria->addItems({"Playlist", "Recomendado", "Favorito", "Infantil", "Instrumental"});

    comboTipo =new QComboBox();
    comboTipo->addItems({"Single", "EP", "Álbum"});

    txtDescripcion= new QTextEdit();
    txtDescripcion->setPlaceholderText("Descripción de la canción...");

    btnSubir= new QPushButton("Subir Canción");

    // Layout vertical para los combos (derecha)
    QVBoxLayout *layoutCombos =new QVBoxLayout();
    layoutCombos->addWidget(new QLabel("Género:"));
    layoutCombos->addWidget(comboGenero);
    layoutCombos->addWidget(new QLabel("Categoría:"));
    layoutCombos->addWidget(comboCategoria);

    // Layout horizontal para imagen + combos
    QHBoxLayout *layoutImagenYCombos = new QHBoxLayout();
    layoutImagenYCombos->addWidget(lblVistaPreviaPortada);
    layoutImagenYCombos->addLayout(layoutCombos);

    // Ensamblar todo
    layoutPrincipal->addWidget(lblAudioSeleccionado);
    layoutPrincipal->addWidget(btnSeleccionarAudio);
    layoutPrincipal->addWidget(lblPortadaSeleccionada);
    layoutPrincipal->addWidget(btnSeleccionarPortada);
    layoutPrincipal->addWidget(lblTitulo);
    layoutPrincipal->addWidget(lblDuracion);
    layoutPrincipal->addLayout(layoutImagenYCombos);
    layoutPrincipal->addWidget(new QLabel("Tipo:"));
    layoutPrincipal->addWidget(comboTipo);
    layoutPrincipal->addWidget(new QLabel("Descripción:"));
    layoutPrincipal->addWidget(txtDescripcion);
    layoutPrincipal->addWidget(btnSubir);

    connect(btnSeleccionarAudio, &QPushButton::clicked, this, &PantallaAdmin::seleccionarArchivoAudio);
    connect(btnSeleccionarPortada, &QPushButton::clicked, this, &PantallaAdmin::seleccionarPortada);
    connect(btnSubir, &QPushButton::clicked, this, &PantallaAdmin::subirCancion);

}

void PantallaAdmin::seleccionarArchivoAudio()
{

    rutaAudio=QFileDialog::getOpenFileName(this, "Seleccionar archivo de audio", "", "Audio (*.mp3 *.mp4)");
    if(!rutaAudio.isEmpty())
    {

        QMediaPlayer*player=new QMediaPlayer(this);
        player->setSource(QUrl::fromLocalFile(rutaAudio));

        connect(player,&QMediaPlayer::mediaStatusChanged,this,[=](QMediaPlayer::MediaStatus status){

            if(status==QMediaPlayer::LoadedMedia)
            {

                qint64 duracionMS=player->duration();
                int segundos=(duracionMS/1000)%60;
                int minutos=(duracionMS/60000)%60;
                int horas=(duracionMS/3600000);
                duracion=QString("%1:%2:%3").arg(horas,2,10,QChar('0')).arg(minutos,2,10,QChar('0')).arg(segundos,2,10,QChar('0'));

                //con el metadato del titulo
                QMediaMetaData meta=player->metaData();
                QString metaTitulo=meta.stringValue(QMediaMetaData::Title);
                QString metaDescripcion=meta.stringValue(QMediaMetaData::Description);

                QFileInfo info(rutaAudio);
                titulo=metaTitulo.isEmpty()?info.baseName():metaTitulo;
                //txtDescripcion->setReadOnly(true);

                if(!metaDescripcion.isEmpty())
                {

                    txtDescripcion->setPlainText(metaDescripcion);

                }

                lblAudioSeleccionado->setText("Audio Seleccionado: "+rutaAudio);
                lblTitulo->setText("Título: "+titulo);
                lblDuracion->setText("Duración: "+duracion);

                player->deleteLater();

            }

        });

        player->setLoops(1);

    }
}

void PantallaAdmin::seleccionarPortada()
{

    rutaImagen=QFileDialog::getOpenFileName(this, "Seleccionar portada", "", "Imágenes (*.jpg *.png *.jpeg)");
    if(!rutaImagen.isEmpty())
    {

        lblPortadaSeleccionada->setText("Portada seleccionada: " + rutaImagen);

        //con esto mostramos a la miniatura
        QPixmap pixmap(rutaImagen);
        lblVistaPreviaPortada->setPixmap(pixmap.scaled(lblVistaPreviaPortada->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));

    }

}

void PantallaAdmin::subirCancion()
{
    if(rutaAudio.isEmpty()||rutaImagen.isEmpty())
    {

        QMessageBox::warning(this, "Campos Incompletos", "Selecciona el archivo de audio y la portada.");
        return;

    }

    QString genero =comboGenero->currentText();
    QString categoria= comboCategoria->currentText();
    QString tipo =comboTipo->currentText();
    QString descripcion= txtDescripcion->toPlainText();

    GestorCanciones gestor;
    int idCancion =gestor.generarId();

    Cancion c(idCancion, titulo, artista.getId(), genero, categoria, tipo, descripcion,rutaAudio, rutaImagen, duracion, QDate::currentDate(), true);

    if(gestor.guardarCancion(c))
    {

        QMessageBox::information(this, "Éxito", "¡Canción subida correctamente!");
        rutaAudio.clear();
        rutaImagen.clear();
        lblAudioSeleccionado->setText("Archivo de audio: (No seleccionado)");
        lblPortadaSeleccionada->setText("Portada: (No seleccionada)");
        lblTitulo->setText("Título: -");
        lblDuracion->setText("Duración: -");
        txtDescripcion->clear();

    }else{

        QMessageBox::critical(this, "Error", "No se pudo guardar la canción.");

    }
}
