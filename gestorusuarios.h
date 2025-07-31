#ifndef GESTORUSUARIOS_H
#define GESTORUSUARIOS_H

#include"usuario.h"
#include<QVector>
#include<QDate>

class GestorUsuarios
{

private:

    QString ArchivoUsuarios="usuarios.dat";

    int generarNuevoId(); //Busca el ultimo ID y genera el siguiente


public:
    GestorUsuarios();

    bool registrarUsuario(const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QString &generoFavorito,const QString &rutaFoto,const QDate& fecha ,bool esAdmin);
    QVector<Usuario> leerUsuarios();
    bool validarLogin(const QString &nombreUsuario, const QString &contrasena, Usuario &usuarioEncontrado);

};

#endif // GESTORUSUARIOS_H
