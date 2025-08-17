#ifndef GESTORUSUARIOS_H
#define GESTORUSUARIOS_H

#include"usuario.h"
#include<QVector>
#include<QDate>
#include<QHash>

/*

Lo nuevo que agregue

Ahora GestorUsuarios mantiene tres indices en memoria (QHash) para validar duplicados en O(1):

idxUsuario: nombreDeUsuario → id

idxCorreo: correo → id

idxNombreReal: nombre real → id

Las claves se normalizan con trimmed().toLower() para que “ Juan ” y “juan” cuenten como lo mismo.

*/

class GestorUsuarios
{

private:

    QString ArchivoUsuarios="usuarios.dat";
    //Indices hash en memoria (claves normalizadas a minusculas y trimmed)
    QHash<QString,int>idxUsuario;     // clave: usuario -> id
    QHash<QString,int>idxCorreo;      // clave: correo  -> id
    QHash<QString,int>idxNombreReal;  // clave: nombre real -> id

    //Normalizacion consistente
    static QString norm(const QString& s){return s.trimmed().toLower();}

public:
    GestorUsuarios();

    bool registrarUsuario(const QString &nombreReal,const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QString &generoFavorito,const QString &rutaFoto,const QDate& fecha ,const QString &correo,bool esAdmin);
    QVector<Usuario> leerUsuarios();
    int generarNuevoId(); //Busca el ultimo ID y genera el siguiente
    bool validarLogin(const QString &nombreUsuario, const QString &contrasena, Usuario &usuarioEncontrado);
    //construye el indice hash desde el archivo
    void construirIndice();

    //helpers 0(1)
    bool existePorUsuario(const QString& nombreUsuario)const;
    bool existePorCorreo(const QString& correo)const;
    bool existePorNombreReal(const QString& nombreReal)const;

    //para exponer el hashCode
    static quint32 djb2(const QString& s);

};

#endif // GESTORUSUARIOS_H
