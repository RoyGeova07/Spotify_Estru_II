#include "gestorusuarios.h"
#include<QFile>
#include<QDataStream>
#include<QDebug>
#include"usuario.h"
#include<QDir>
#include<QMessageBox>

//QDataStream me servira para serializar y deserializar datos binarios desde y hacia un objeto QIODevice

GestorUsuarios::GestorUsuarios(){construirIndice();}

void GestorUsuarios::construirIndice()
{

    idxUsuario.clear();
    idxCorreo.clear();
    idxNombreReal.clear();

    QFile archivo(ArchivoUsuarios);
    if(!archivo.open(QIODevice::ReadOnly))return;

    QDataStream in(&archivo);
    while(!in.atEnd())
    {

        int id;
        QString NombreReal,nombre,contra,genero,foto,correoElectronico;
        QDate fecha,fechaNacimiento;
        bool esAdmin,activo;

        in>>id>>NombreReal>>nombre>>contra>>fechaNacimiento>>fecha>>genero>>foto>>correoElectronico>>esAdmin>>activo;

        //Indexar solo usuarios activos; si quieres indexar tambien inactivos, quita el if.
        if(activo)
        {

            idxUsuario.insert(norm(nombre),id);
            idxCorreo.insert(norm(correoElectronico),id);
            idxNombreReal.insert(norm(NombreReal),id);

        }
    }

    archivo.close();

}

bool GestorUsuarios::existePorUsuario(const QString &nombreUsuario) const
{

    return idxUsuario.contains(norm(nombreUsuario));

}

bool GestorUsuarios::existePorCorreo(const QString& correo) const
{

    return idxCorreo.contains(norm(correo));

}
bool GestorUsuarios::existePorNombreReal(const QString& nombreReal) const
{

    return idxNombreReal.contains(norm(nombreReal));

}


quint32 GestorUsuarios::djb2(const QString &s)
{

    QByteArray a=s.toUtf8();
    quint32 hash=5381;
    for(unsigned char c:a)
        hash=((hash<<5)+hash)+c;// hash * 33 + c
    return hash;

}

int GestorUsuarios::generarNuevoId()
{

    //Mas robusto:max id+1
    int maxId=0;
    QFile archivo(ArchivoUsuarios);
    if(!archivo.open(QIODevice::ReadOnly))return 1;

    QDataStream in(&archivo);
    while(!in.atEnd())
    {

        int id;
        QString NombreReal,nombre,contra,genero,foto,correoElectronico;
        QDate fecha,fechaNacimiento;
        bool esAdmin,activo;
        in>>id>>NombreReal>>nombre>>contra>>fechaNacimiento>>fecha>>genero>>foto>>correoElectronico>>esAdmin>>activo;
        if(id>maxId)maxId=id;
    }
    archivo.close();
    return maxId+1;

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
    //asegurar de tener el indice cargado
    if(idxUsuario.isEmpty()&&idxCorreo.isEmpty()&&idxNombreReal.isEmpty())
        construirIndice();

    //Validacion O(1) con hash
    const QString kUser=norm(nombreUsuario);
    const QString kNombre=norm(NombreReal);
    const QString kMail=norm(CorreoElectronico);

    if(existePorUsuario(kUser))return false;
    if(existePorNombreReal(kNombre))return false;
    if(existePorCorreo(kMail))return false;

    const int NuevoID=generarNuevoId();

    Usuario nuevo(NuevoID, NombreReal, nombreUsuario, contrasena,fechaNacimiento, fecha, generoFavorito, rutaFoto,CorreoElectronico, esAdmin, true);

    QFile archivo(ArchivoUsuarios);
    if(!archivo.open(QIODevice::Append))
    {

        qWarning()<<"No se pudo abrir el archivo para escribir.";
        return false;

    }

    QDataStream out(&archivo);
    out<<nuevo.getId()<<nuevo.getNombreReal()<<nuevo.getNombreUsuario()<<nuevo.getContrasena()<<nuevo.getFechaNacimiento()<<nuevo.getFechaRegistro()<<nuevo.getGeneroFavorito()<<nuevo.getRutaFoto()<<nuevo.getCorreo()<<nuevo.getEsAdmin()<<nuevo.estaActivo();
    archivo.close();

    // Actualizar el indice en memoria
    idxUsuario.insert(kUser,NuevoID);
    idxNombreReal.insert(kNombre,NuevoID);
    idxCorreo.insert(kMail,NuevoID);

    // Asegurar carpeta de usuario
    QString baseDir="Publico";
    QDir dir;
    if(!dir.exists(baseDir))dir.mkpath(baseDir);

    const QString CarpetaUsuario=baseDir+"/Usuario_"+nombreUsuario;
    if(!dir.exists(CarpetaUsuario))dir.mkpath(CarpetaUsuario);

    return true;

}

bool GestorUsuarios::validarLogin(const QString &nombreUsuario, const QString &contrasena, Usuario &usuarioEncontrado)
{

    if(!existePorUsuario(nombreUsuario))return false;

    // Recuperar del archivo
    QVector<Usuario>usuarios=leerUsuarios();
    for(const Usuario&u:usuarios)
    {
        if(u.getNombreUsuario()==nombreUsuario&&u.getContrasena()==contrasena&&u.estaActivo())
        {

            usuarioEncontrado=u;
            return true;

        }
    }

    return false;

}
