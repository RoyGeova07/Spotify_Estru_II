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
    ~ControlReproduccion();//DESTRUCTOR PARA LIBERAR NODOS

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

    void indiceActualizado(int nuevoIndice);

private slots:

    void ManejarFinalCancion();

private:
    int obtenerSiguienteIndice();

    QMediaPlayer*reproductor;
    QAudioOutput* salidaAudio;

    //CONSTRUIR LISTA DOBLEMENTE ENLAZADA PARA EL SIGUIENTE Y ANTERIOR
    struct Nodo
    {

        int index;//indice dentro de listaCanciones
        Nodo* prev;
        Nodo* next;

    };

    void construirLista();// Reconstruye la lista circular a partir de listaCanciones
    void limpiarLista();//Libera todos los nodos y resetea punteros/estado
    void irANodo(Nodo* n);//Cambia el nodo "actual": setea source en QMediaPlayer, da play y emite señal de indice actualizado
    Nodo*nodoAleatorio() const;

    QVector<Cancion>listaCanciones;
    QVector<Nodo*>mapaIndices;//Acceso O(1): index -> nodo
    Nodo*head=nullptr;
    Nodo*actual=nullptr;

    int  indiceActual=-1;//Indice actual dentro de listaCanciones
    bool aleatorio=false;
    bool repetir=false;



};

#endif // CONTROLREPRODUCCION_H
