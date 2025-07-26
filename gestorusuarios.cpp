#include "gestorusuarios.h"
#include<QFile>
#include<QDataStream>
#include<QDebug>
#include"usuario.h"

//QDataStream me servira para serializar y deserializar datos binarios desde y hacia un objeto QIODevice

GestorUsuarios::GestorUsuarios() {}

int GestorUsuarios::generarNuevoId()
{

    QVector<Usuario>usuarios=leerUsuarios();
    if(usuarios.isEmpty())return 1;
    return usuarios.last().getId()+1;

}


QVector<Usuario> GestorUsuarios::leerUsuarios()
{
    QVector<Usuario> usuarios;
    QFile archivo(ArchivoUsuarios);
    if(!archivo.open(QIODevice::ReadOnly)) return usuarios;

    QDataStream in(&archivo);
    while(!in.atEnd())
    {
        int id;
        QString nombre, contra, genero, foto;
        QDate fecha;
        bool esAdmin, activo;

        in >> id >> nombre >> contra >> fecha >> genero >> foto >> esAdmin >> activo;
        Usuario u(id, nombre, contra, fecha, genero, foto, esAdmin, activo);
        usuarios.append(u);
    }

    archivo.close();
    return usuarios;
}


bool GestorUsuarios::registrarUsuario(const QString &nombreUsuario, const QString &contrasena,const QString &generoFavorito, const QString &rutaFoto,const QDate &fecha, bool esAdmin)
{
    QVector<Usuario>usuarios=leerUsuarios();


    for(const Usuario&u:usuarios)
    {
        if(u.getNombreUsuario().toLower()==nombreUsuario.toLower())
        {

            qWarning()<<"El nombre de usuario ya esta en uso.";
            return false;

        }
    }

    int NuevoID=generarNuevoId();

    for(const Usuario &u : usuarios)
    {
        if(u.getId()==NuevoID)
        {
            qWarning()<<"Conflicto de ID detectado.";
            return false;
        }
    }

    Usuario nuevo(NuevoID, nombreUsuario, contrasena, fecha, generoFavorito, rutaFoto, esAdmin, true);

    QFile archivo(ArchivoUsuarios);
    if(!archivo.open(QIODevice::Append))
    {

        qWarning() << "No se pudo abrir el archivo para escribir.";
        return false;

    }

    QDataStream out(&archivo);
    out << nuevo.getId()<< nuevo.getNombreUsuario()<< nuevo.getContrasena()<< nuevo.getFechaRegistro()<< nuevo.getGeneroFavorito()<< nuevo.getRutaFoto()<< nuevo.getEsAdmin()<< nuevo.estaActivo();

    archivo.close();
    return true;
}

bool GestorUsuarios::validarLogin(const QString &nombreUsuario, const QString &contrasena, Usuario &usuarioEncontrado)
{

    QVector<Usuario>usuarios=leerUsuarios();
    for(const Usuario &u:usuarios)
    {

        if(u.getNombreUsuario()==nombreUsuario&&u.getContrasena()==contrasena&&u.estaActivo())
        {

            usuarioEncontrado=u;
            return true;

        }

    }
    return false;

}
