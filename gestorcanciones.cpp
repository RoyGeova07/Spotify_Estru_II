#include "gestorcanciones.h"
#include<QFile>
#include<QDataStream>
#include<QDebug>


GestorCanciones::GestorCanciones() {}

bool GestorCanciones::guardarCancion(const Cancion &c)
{

    QFile file(archivo);
    if(!file.open(QIODevice::Append))
    {

        qWarning()<<"NO SE PUDO ABRIR EL ARCHIVO";
        return false;

    }
    QDataStream out(&file);
    out<<c.getId()<<c.getTitulo()<<c.getNombreArtista()<< c.getGenero()<<c.getCategoria()<< c.getTipo() <<c.getDescripcion()<<c.getRutaAudio()<<c.getRutaImagen()<<c.getDuracion()<<c.getFechaCarga()<<c.estaActiva();

    file.close();
    return true;

}

QVector<Cancion>GestorCanciones::leerCanciones()
{

   QVector<Cancion>canciones;
    QFile file(archivo);
   if(!file.open(QIODevice::ReadOnly))return canciones;

   QDataStream in(&file);
   while(!in.atEnd())
   {

       int id;
       QString titulo,NombreArtista,genero,categoria,tipo,descripcion,rutaAudio,rutaImagen,duracion;
       QDate fecha;
       bool Activo;

       in>>id>>titulo>>NombreArtista>>genero>>categoria>>tipo>>descripcion>>rutaAudio>>rutaImagen>>duracion>>fecha>>Activo;

       Cancion c(id,titulo,NombreArtista,genero,categoria,tipo,descripcion,rutaAudio,rutaImagen,duracion,fecha,Activo);
       canciones.append(c);

   }
   file.close();
   return canciones;

}

int GestorCanciones::generarId()
{

    QVector<Cancion>canciones=leerCanciones();
    int MaxId=0;
    for(const Cancion &c:canciones)
    {

        if(c.getId()>MaxId)
        {

            MaxId=c.getId();

        }

    }
    return MaxId+1;

}

bool GestorCanciones::CancionDuplicada(const Cancion &NuevaCancion)
{

    QVector<Cancion>canciones=leerCanciones();

    for(const Cancion&c:canciones)
    {

        if(c.getTitulo()==NuevaCancion.getTitulo()||c.getRutaAudio()==NuevaCancion.getRutaAudio()||c.getRutaImagen()==NuevaCancion.getRutaImagen())
        {

            return true;//YA EXISTE

        }

    }
    return false;

}
