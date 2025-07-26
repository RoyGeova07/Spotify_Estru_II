#include "gestorartistas.h"
#include<QFile>
#include<QDataStream>
#include<QDebug>

GestorArtistas::GestorArtistas() {}


bool GestorArtistas::GuardarArtista(const Artista &a)
{

    QFile file(Archivo);
    if(!file.open(QIODevice::Append))
    {

        qWarning()<<"No se pudo abrir artistas.dat";
        return false;

    }

    QDataStream out(&file);
    out << a.getId()<< a.getNombreArtistico()<< a.getNombreReal()<< a.getPais()<< a.getGenero()<< a.getBiografia()<< a.getContrasena()<< a.getFechaRegistro()<< a.getRutaImagen()<< a.estaActivo();

    file.close();
    return true;

}

bool GestorArtistas::registrarArtista(int idUsuario, const QString &nombreArtistico, const QString &nombreReal, const QString &pais, const QString &generoMusical, const QString &biografia, const QString &contrasena, const QString &rutaImagen)
{

    QVector<Artista>artistas=leerArtista();
    for(const Artista &a:artistas)
    {

        if(a.getId()==idUsuario)
        {

            qWarning()<<"El usuario ya esta registrado como artista.";
            return false;

        }

        if(a.getNombreArtistico().toLower()==nombreArtistico.toLower())
        {

            qWarning()<<"Ya existe un artista con ese nombre artistico.";
            return false;

        }

    }

    Artista artista(idUsuario, nombreArtistico, nombreReal, pais, generoMusical,biografia, contrasena, rutaImagen, true);

    return GuardarArtista(artista);

}

QVector<Artista> GestorArtistas::leerArtista()
{

    QVector<Artista> artistas;
    QFile file(Archivo);
    if(!file.open(QIODevice::ReadOnly)) return artistas;

    QDataStream in(&file);
    while(!in.atEnd())
    {
        int id;
        QString artistico, real, pais, genero, bio, contra, ruta;
        QDate fecha;
        bool activo;

        in >> id >> artistico >> real >> pais >> genero >> bio >> contra >> fecha >> ruta >> activo;
        Artista a(id, artistico, real, pais, genero, bio, contra, ruta, activo);
        artistas.append(a);

    }

    file.close();
    return artistas;
}

bool GestorArtistas::validarLogIn(const QString &nombreReal, const QString &contrasena, Artista &usuarioEncontrado)
{

    QVector<Artista>artista=leerArtista();
    for(const Artista&a:artista)
    {

        if(a.getNombreReal()==nombreReal&&a.getContrasena()==contrasena&&a.estaActivo())
        {

            usuarioEncontrado=a;
            return true;

        }

    }
    return false;

}
