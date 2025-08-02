#include "gestorartistas.h"
#include<QFile>
#include<QDataStream>
#include<QDebug>
#include<QMessageBox>

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
    out << a.getId()<< a.getNombreArtistico()<< a.getNombreReal()<< a.getPais()<< a.getGenero()<< a.getBiografia()<< a.getContrasena()<< a.getFechaRegistro()<< a.getRutaImagen()<< a.estaActivo()<<a.getFechaNacimiento();

    file.close();
    return true;

}

bool GestorArtistas::registrarArtista(int idUsuario, const QString &nombreArtistico, const QString &nombreReal, const QString &pais, const QString &generoMusical, const QString &biografia, const QString &contrasena, const QString &rutaImagen,const QDate &fechaNacimiento)
{

    QVector<Artista>artistas=leerArtista();
    QString artisticoLimpio=nombreArtistico.trimmed().toLower();
    QString realLimpio=nombreReal.trimmed().toLower();

    //LEA A TODOS LOS ARTISTAS DEL ARCHIVO BINARIOSZZZ
    for(const Artista &a:artistas)
    {

        if(a.getId()==idUsuario)
        {

            QMessageBox::warning(nullptr,"Registro fallido","ID igual.");
            return false;

        }

        if(a.getNombreArtistico().trimmed().toLower()==artisticoLimpio)
        {

            QMessageBox::warning(nullptr,"Registro fallido","Este usuario ya esta registrado con ese nombre artistico.");
            return false;

        }

        if(a.getNombreReal().trimmed().toLower()==realLimpio)
        {

            QMessageBox::warning(nullptr,"Registro fallido","Este usuario ya esta registrado con ese nombre.");
            return false;

        }

    }

    Artista artista(idUsuario, nombreArtistico, nombreReal, pais, generoMusical,biografia, contrasena, rutaImagen, true,fechaNacimiento);

    return GuardarArtista(artista);

}

QVector<Artista> GestorArtistas::leerArtista()
{

    QVector<Artista> artistas;
    QFile file(Archivo);
    if(!file.open(QIODevice::ReadOnly)) return artistas;

    QDataStream in(&file);
    while(!in.atEnd())//HASTA LLEGAR AL FINAL DEL ARCHIVO
    {
        int id;
        QString artistico, real, pais, genero, bio, contra, ruta;
        QDate fecha,fechaNacimiento;
        bool activo;

        in >> id >> artistico >> real >> pais >> genero >> bio >> contra >> fecha >> ruta >> activo>>fechaNacimiento;
        Artista a(id, artistico, real, pais, genero, bio, contra, ruta, activo,fechaNacimiento);
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

        if(a.getNombreReal().trimmed().toLower()==nombreReal.trimmed().toLower()&&a.getContrasena()==contrasena&&a.estaActivo())
        {

            usuarioEncontrado=a;
            return true;

        }

    }
    return false;

}
