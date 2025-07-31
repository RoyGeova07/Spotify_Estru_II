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
    out<<c.getId()<<c.getTitulo()<<c.getIdArtista()<< c.getGenero()<<c.getCategoria()<< c.getTipo() <<c.getDescripcion()<<c.getRutaAudio()<<c.getRutaImagen()<<c.getDuracion()<<c.getFechaCarga()<<c.estaActiva();

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

       int id,idArtista;
       QString titulo,genero,categoria,tipo,descripcion,rutaAudio,rutaImagen,duracion;
       QDate fecha;
       bool Activo;

       in>>id>>titulo>>idArtista>>genero>>categoria>>tipo>>descripcion>>rutaAudio>>rutaImagen>>duracion>>fecha>>Activo;

       Cancion c(id,titulo,idArtista,genero,categoria,tipo,descripcion,rutaAudio,rutaImagen,duracion,fecha,Activo);
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
