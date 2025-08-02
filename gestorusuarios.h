#ifndef GESTORUSUARIOS_H
#define GESTORUSUARIOS_H

#include"usuario.h"
#include<QVector>
#include<QDate>

class GestorUsuarios
{

private:

    QString ArchivoUsuarios="usuarios.dat";

public:
    GestorUsuarios();

    bool registrarUsuario(const QString &nombreReal,const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QString &generoFavorito,const QString &rutaFoto,const QDate& fecha ,const QString &correo,bool esAdmin);
    QVector<Usuario> leerUsuarios();
    int generarNuevoId(); //Busca el ultimo ID y genera el siguiente
    bool validarLogin(const QString &nombreUsuario, const QString &contrasena, Usuario &usuarioEncontrado);

};

#endif // GESTORUSUARIOS_H
