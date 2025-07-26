#ifndef GESTORARTISTAS_H
#define GESTORARTISTAS_H

#include"artista.h"
#include<QVector>


class GestorArtistas
{

private:

    QString Archivo="artistas.dat";

public:
    GestorArtistas();

    bool registrarArtista(int idUsuario, const QString &nombreArtistico,const QString &nombreReal, const QString &pais,const QString &generoMusical, const QString &biografia,const QString &contrasena, const QString &rutaImagen);
    bool validarLogIn(const QString &nombreReal, const QString &contrasena, Artista &usuarioEncontrado);

    bool GuardarArtista(const Artista &a);
    QVector<Artista>leerArtista();

};

#endif // GESTORARTISTAS_H
