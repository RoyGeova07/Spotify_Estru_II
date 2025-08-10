#ifndef GESTORARTISTAS_H
#define GESTORARTISTAS_H

#include"artista.h"
#include<QVector>
#include<QDate>


class GestorArtistas
{

private:

    QString Archivo="artistas.dat";

public:
    GestorArtistas();

    bool registrarArtista(int idUsuario, const QString &nombreArtistico,const QString &nombreReal, const QString &pais,const QString &generoMusical, const QString &biografia,const QString &contrasena, const QString &rutaImagen,const QDate &fechaNacimiento);
    bool validarLogIn(const QString &nombreReal, const QString &contrasena, Artista &usuarioEncontrado);

    bool GuardarArtista(const Artista &a);
    QVector<Artista>leerArtista();

    bool existeNombreArtistico(const QString& nombre,int excluirId=-1);

    //Reemplaza al artista por ID con los nuevos datos
    bool actualizarArtista(const Artista& nuevo);

};

#endif // GESTORARTISTAS_H
