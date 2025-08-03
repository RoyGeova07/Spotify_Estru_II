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

};

#endif // GESTORCANCIONES_H
