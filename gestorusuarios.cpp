#include "gestorusuarios.h"
#include<QFile>
#include<QDataStream>
#include<QDebug>
#include"usuario.h"
#include<QDir>
#include<QMessageBox>

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
        QString NombreReal,nombre, contra, genero, foto,correoElectronico;
        QDate fecha,fechaNacimiento;
        bool esAdmin, activo;

        in >> id >> NombreReal>>nombre >> contra >> fechaNacimiento>>fecha >> genero >> foto >> correoElectronico>>esAdmin >> activo;

        Usuario u(id, NombreReal,nombre, contra, fechaNacimiento,fecha, genero, foto, correoElectronico,esAdmin, activo);
        usuarios.append(u);
    }

    archivo.close();
    return usuarios;
}


bool GestorUsuarios::registrarUsuario(const QString &NombreReal,const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QString &generoFavorito, const QString &rutaFoto,const QDate &fecha,const QString &CorreoElectronico, bool esAdmin)
{
    QVector<Usuario>usuarios=leerUsuarios();

    //LEA A TODOS LOS USUARIOS DEL ARCHIVO BINARIOZZZZ
    QString nombreLimpio=nombreUsuario.trimmed().toLower();


    for(const Usuario&u:usuarios)
    {

        if(u.getNombreUsuario().trimmed().toLower()==nombreLimpio)
        {

            return false;

        }

        if(u.getNombreReal().trimmed().toLower()==NombreReal)
        {

            return false;

        }

        if(u.getCorreo().trimmed().toLower()==CorreoElectronico)
        {

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

    Usuario nuevo(NuevoID,NombreReal ,nombreUsuario, contrasena,fechaNacimiento,fecha, generoFavorito, rutaFoto, CorreoElectronico,esAdmin, true);

    QFile archivo(ArchivoUsuarios);
    if(!archivo.open(QIODevice::Append))
    {

        qWarning() << "No se pudo abrir el archivo para escribir.";
        return false;

    }

    QDataStream out(&archivo);
    out << nuevo.getId()<<nuevo.getNombreReal() <<nuevo.getNombreUsuario()<< nuevo.getContrasena()<< nuevo.getFechaNacimiento()<<nuevo.getFechaRegistro()<< nuevo.getGeneroFavorito()<< nuevo.getRutaFoto()<< nuevo.getCorreo()<<nuevo.getEsAdmin()<<nuevo.estaActivo();

    archivo.close();

    QString baseDir="Publico";
    QDir dir;

    if(!dir.exists(baseDir))
    {

        dir.mkdir(baseDir);

    }

    QString CarpetaUsuario=baseDir+"/Usuario_"+nombreUsuario;

    if(!dir.exists(CarpetaUsuario))
    {

        if(dir.mkdir(CarpetaUsuario))
        {

            dir.mkdir(CarpetaUsuario+"/canciones");
            dir.mkdir(CarpetaUsuario+"/playlist");
            dir.mkdir(CarpetaUsuario+"/favoritos");

        }else{

            qWarning()<<"NO SE PUDO CREAR LA CARPETA DE USUARIO";

        }

    }

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
