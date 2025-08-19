#include "gestorcanciones.h"
#include<QFile>
#include<QDataStream>
#include<QDebug>
#include <QDir>
#include <QFileInfo>


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

    int maxId=0;
    //1) Max en canciones.dat
    const QVector<Cancion>canciones=leerCanciones();
    for(const Cancion &c:canciones)
        if(c.getId()>maxId) maxId=c.getId();

    //2)Max en reproducciones.dat  [MAGIC/version por registro]
    {
        QFile f("reproducciones.dat");
        if(f.exists()&&f.open(QIODevice::ReadOnly))
        {

            QDataStream in(&f);
            in.setVersion(QDataStream::Qt_5_15);
            while(!in.atEnd())
            {
                quint32 magic=0; quint16 ver=0;
                in>>magic>>ver;
                if(in.status()!=QDataStream::Ok)break;

                quint32 userId=0, songId=0;
                quint64 epoch=0;
                quint32 msPlayed=0,durMs=0;
                // Orden usado al escribir: userId, songId, epochMs, msPlayed, durMs
                in>>userId>>songId>>epoch>>msPlayed>>durMs;

                if(in.status()!=QDataStream::Ok)break;
                if(static_cast<int>(songId)>maxId)maxId=static_cast<int>(songId);

            }

        }
    }

    //3)Max en calificaciones.dat
    {
        QFile f("calificaciones.dat");
        if(f.exists()&&f.open(QIODevice::ReadOnly))
        {

            QDataStream in(&f);
            in.setVersion(QDataStream::Qt_5_15);
            while(!in.atEnd())
            {

                quint32 magic=0;quint16 ver=0;
                in>>magic>>ver;
                if(in.status()!=QDataStream::Ok)break;

                quint32 userId=0, songId=0;
                quint8 rating=0;
                quint64 epoch=0;
                // Orden usado al escribir: userId, songId, rating, epochMs
                in >> userId >> songId>>rating>>epoch;

                if(in.status() != QDataStream::Ok) break;
                if(static_cast<int>(songId)>maxId) maxId=static_cast<int>(songId);
            }

        }
    }

    return maxId+1;
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

//Los enum como Genero, Categoria y Tipo se deben guardar como int usando static_cast<int>
bool GestorCanciones::eliminarCancionPorTitulo(const QString &titulo)
{


    QVector<Cancion>canciones=leerCanciones();
    bool eliminado=false;

    for(int i=0;i<canciones.size();i++)
    {

        if(canciones[i].getTitulo().trimmed().toLower()==titulo.trimmed().toLower())
        {

            canciones.removeAt(i);
            eliminado=true;
            break;

        }

    }
    if(!eliminado)return false;

    QFile file("canciones.dat");
    if(!file.open(QIODevice::WriteOnly))return false;

    QDataStream out(&file);
    for(const Cancion&c:canciones)
    {

        out<<c.getReproducciones()<<c.getId()<<c.getTitulo()<< c.getNombreArtista()<< static_cast<int>(c.getGenero())<< static_cast<int>(c.getCategoria())<< static_cast<int>(c.getTipo())<<c.getDescripcion()<<c.getRutaAudio()<<c.getRutaImagen()<<c.getDuracion()<<c.getFechaCarga()<<c.estaActiva();

    }
    file.close();
    return true;

}

bool GestorCanciones::eliminarCancionPorTituloYArtista(const QString &titulo, const QString &artista)
{

    QVector<Cancion>canciones=leerCanciones();
    bool eliminado=false;

    for(int i=0; i<canciones.size();++i)
    {

        if(canciones[i].getTitulo().trimmed().toLower()==titulo.trimmed().toLower())
        {

            canciones.removeAt(i);
            eliminado=true;
            break;

        }

    }
    if(!eliminado)return false;

    QFile file(archivo);
    if(!file.open(QIODevice::WriteOnly))return false;

    QDataStream out(&file);
    for(const Cancion&c:canciones)
    {

        out<<c.getReproducciones()<<c.getId()<<c.getTitulo()<<c.getNombreArtista()<<generoToString(c.getGenero())<<categoriaToString(c.getCategoria())<<tipoToString(c.getTipo())<<c.getDescripcion()<<c.getRutaAudio()<<c.getRutaImagen()<<c.getDuracion()<<c.getFechaCarga()<<c.estaActiva();

    }
    file.close();
    return true;

}

//Normaliza una ruta para comparacion (sin distinguir mayusculas).
static QString normPath(const QString&p)
{

    //No dependemos de que el archivo exista; limpiamos la ruta y comparamos en lower.
    return QDir::cleanPath(p.trimmed()).toLower();

}

bool GestorCanciones::existeTituloORutaImagen(const QString &titulo, const QString &rutaImagen, int excluirId)
{

    const QVector<Cancion>canciones=leerCanciones();
    const QString t=titulo.trimmed().toLower();
    const QString r=normPath(rutaImagen);

    for(const Cancion&c:canciones)
    {

        if(excluirId!=-1&&c.getId()==excluirId)continue;

        const bool tituloDup=(!t.isEmpty()&&c.getTitulo().trimmed().toLower()==t);

        const bool caratulaDup=(!r.isEmpty()&&c.getRutaImagen().trimmed().toLower()==r);

        if(tituloDup||caratulaDup)return true;

    }
    return false;

}


bool GestorCanciones::ActualizarCancionPorId(const Cancion &nueva)
{

    QVector<Cancion>canciones=leerCanciones();

    // 1) Valida unicidad global (excepto la propia).
    if(existeTituloORutaImagen(nueva.getTitulo(),nueva.getRutaImagen(),nueva.getId()))
    {

        return false;//DUPLICADO NO SE GUARDAAAA

    }

    //2) Reemplaza y persiste
    bool Modificado=false;

    for(Cancion&c:canciones)
    {

        if(c.getId()==nueva.getId())
        {

            c=nueva;//AQUI REEMPLAZA EL OBJETO ENTERO
            Modificado=true;
            break;

        }

    }
    if(!Modificado)return false;

    QFile file(archivo);
    if(!file.open(QIODevice::WriteOnly))return false;

    QDataStream out(&file);
    for(const Cancion&c:canciones)
    {

        out<<c.getReproducciones()<<c.getId()<<c.getTitulo()<<c.getNombreArtista()<<generoToString(c.getGenero())<<categoriaToString(c.getCategoria())<<tipoToString(c.getTipo())<<c.getDescripcion()<<c.getRutaAudio()<<c.getRutaImagen()<<c.getDuracion()<<c.getFechaCarga()<<c.estaActiva();

    }
    file.close();
    return true;

}
