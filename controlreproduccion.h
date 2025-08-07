#ifndef CONTROLREPRODUCCION_H
#define CONTROLREPRODUCCION_H

#include<QObject>
#include<QMediaPlayer>
#include"cancion.h"
#include<QAudioOutput>


class ControlReproduccion:public QObject
{

    Q_OBJECT

public:
    ControlReproduccion(QObject*parent=nullptr);

    void setListaCanciones(const QVector<Cancion>&canciones);
    void reproducir(int indice);
    void pausar();
    void detener();
    void siguiente();
    void anterior();
    void reiniciar();
    void activarAleatorio(bool activar);
    void activarRepetir(bool activar);
    void play();

    int getIndiceActual() const;
    QMediaPlayer*getReproductor() const;

signals:

    void cancionTerminada();
    void indiceActualizado(int nuevoIndice);

private slots:

    void ManejarFinalCancion();

private:

    QVector<Cancion>listaCanciones;
    QMediaPlayer*reproductor;
    QAudioOutput* salidaAudio;
    int indiceActual;
    bool aleatorio;
    bool repetir;

    int obtenerSiguienteIndice();


};

#endif // CONTROLREPRODUCCION_H
