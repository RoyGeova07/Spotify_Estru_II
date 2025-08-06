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
    out<<c.getReproducciones()<<c.getId()<<c.getTitulo()<<c.getNombreArtista()<< generoToString(c.getGenero())<<categoriaToString(c.getCategoria())<<tipoToString( c.getTipo()) <<c.getDescripcion()<<c.getRutaAudio()<<c.getRutaImagen()<<c.getDuracion()<<c.getFechaCarga()<<c.estaActiva();

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

       int id,reproducciones;
       QString titulo,NombreArtista,descripcion,rutaAudio,rutaImagen,duracion;
       QDate fecha;
       bool Activo;

       QString generoStr,categoriaStr,tipoStr;
       in>>reproducciones>>id>>titulo>>NombreArtista>>generoStr>>categoriaStr>>tipoStr>>descripcion>>rutaAudio>>rutaImagen>>duracion>>fecha>>Activo;

       Genero genero=stringToGenero(generoStr);
       Categoria categoria=stringToCategoria(categoriaStr);
       Tipo tipo=stringToTipo(tipoStr);

       Cancion c(reproducciones,id,titulo,NombreArtista,genero,categoria,tipo,descripcion,rutaAudio,rutaImagen,duracion,fecha,Activo);
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
