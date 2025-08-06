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
//#include"controlreproduccion.h"
#include"usuario.h"
#include"home.h"

class ReproductorMusica:public QWidget
{
    Q_OBJECT

public:

    explicit ReproductorMusica(const QVector<Cancion>&canciones, const Usuario& usuarioActivo,QWidget*parent =nullptr);

private slots:

    // void reproducirCancion(int index);
    // void playPause();
    // void siguiente();
    // void anterior();
    void actualizarPosicion(qint64 position);
    void actualizarDuracion(qint64 duration);
    // void cambiarPosicion(int position);
    // void toggleRepetir();
    // void toggleAleatorio();
    void cerrarReproductor();

private:

    QLabel* lblTitulo;
    QLabel* lblArtista;
    QLabel* lblTipo;
    QLabel* lblReproducciones;
    QVector<Cancion> listaCanciones;
    Usuario usuarioActivo;
    QMediaPlayer* reproductor;
    QListWidget* listaWidget;
    QPushButton* btnPlayPause;
    QPushButton* btnRepetir;
    QPushButton* btnAleatorio;
    QPushButton* btnSiguiente;
    QPushButton* btnAnterior;
    QPushButton* btnGuardarPlaylist;
    QPushButton* btnCerrar;
    QSlider* barraProgreso;
    QLabel* lblTiempoActual;
    QLabel* lblDuracionTotal;
    QLabel* lblCaratula;
    QLabel* lblUsuario;
    int indiceActual;
    //ControlReproduccion control;

    void cargarCancionesEnLista();
    void actualizarEstadoBotones();
    void configurarInterfaz();
    QString formatoTiempo(qint64 milisegundos);
};

#endif // REPRODUCTORMUSICA_H
