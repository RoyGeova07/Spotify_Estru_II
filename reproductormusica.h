#ifndef REPRODUCTORMUSICA_H
#define REPRODUCTORMUSICA_H

#include<QWidget>
#include<QMediaPlayer>
#include<QPushButton>
#include<QSlider>
#include<QLabel>
#include<QListWidget>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include"cancion.h"
#include"usuario.h"
#include"home.h"
#include"controlreproduccion.h"
#include"sliderclickable.h"
#include"gestorreproduccion.h"
#include"gestorcanciones.h"

class ReproductorMusica:public QWidget
{
    Q_OBJECT

public:

    explicit ReproductorMusica(const QVector<Cancion>&canciones, const Usuario& usuarioActivo,QWidget*parent =nullptr,bool modoPlayList=false,const QString&pathPlaylist="");

private slots:

    void actualizarPosicion(qint64 position);
    void actualizarDuracion(qint64 duration);
    void cerrarReproductor();

protected:

    void closeEvent(QCloseEvent *event)override;

private:

    QLabel* lblTitulo;
    QLabel* lblArtista;
    QLabel* lblTipo;
    QLabel* lblReproducciones;
    QVector<Cancion> listaCanciones;
    Usuario usuarioActivo;
    QListWidget* listaWidget;
    QPushButton* btnPlayPause;
    QPushButton* btnRepetir;
    QPushButton* btnAleatorio;
    QPushButton* btnSiguiente;
    QPushButton* btnAnterior;
    QPushButton* btnCerrar;
    SliderClickable*barraProgreso;
    QLabel* lblTiempoActual;
    QLabel* lblDuracionTotal;
    QLabel* lblCaratula;
    QLabel* lblUsuario;
    QLabel*lblDescripcion;
    QLabel*lblGenero;
    QLabel*lblFechaCarga;
    int indiceActual;

    ControlReproduccion*control;

    void cargarCancionesEnLista();
    void actualizarEstadoBotones();
    void configurarInterfaz();
    QString formatoTiempo(qint64 milisegundos);
    void guardarEnPlaylist(int indexCancion);
    bool cancionYaEnPlaylist(const QString& pathDat,const Cancion& c) const;
    QString keyCancion(const Cancion& c) const;  //genera clave unica (hash)

    bool esDesdePlaylist=false;
    QString rutaPlaylistDat;

    void eliminarDePlaylist(int indexFila);

    // Reindexa el "#": 1,2,3... despues de eliminar
    static void reindexarNumeros(QListWidget* lista)
    {

        if(!lista)return;
        for(int i=0;i<lista->count();++i)
        {

            QListWidgetItem*it=lista->item(i);
            QWidget* row=lista->itemWidget(it);
            if(!row)continue;
            if(QLabel* num=row->findChild<QLabel*>("numLbl"))
            {

                num->setText(QString::number(i+1));

            }

        }

    }

    //PARA REGISTRAR EVENTOS
    qint64 posMsActual=0;//POSICION ACTUAL en ms
    qint64 durMsActual=0;//DURACION ACTUAL en ms
    int idxActual=-1;//INDICE DE LA CANCION ACTUAL EN LA LISTA
    GestorReproduccion logger{"reproducciones.dat"};

    void flushEventoActual();//GUARDA EL EVENTO DEL TEMA QUE SE ESTABA OYENDO

    bool estaEnCatalogoVigente(const Cancion&c)const;
    bool navegandoAHome=false;

};

#endif // REPRODUCTORMUSICA_H
