#ifndef GESTORCANCIONES_H
#define GESTORCANCIONES_H

#include"cancion.h"
#include<QVector>

class GestorCanciones
{

private:

    QString archivo="canciones.dat";

public:
    GestorCanciones();


    bool guardarCancion(const Cancion &c);
    QVector<Cancion> leerCanciones();
    int generarId();  // Generador de ID incremental
    bool CancionDuplicada(const Cancion& NuevaCancion);
    bool eliminarCancionPorTitulo(const QString& titulo);
    bool eliminarCancionPorTituloYArtista(const QString& titulo,const QString& artista);
    bool ActualizarCancionPorId(const Cancion&nueva);
    bool existeTituloORutaImagen(const QString& titulo,const QString&rutaImagen,int excluirId=-1);

};

#endif // GESTORCANCIONES_H
