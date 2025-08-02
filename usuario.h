#ifndef USUARIO_H
#define USUARIO_H

#include <QString>
#include <QDate>

class Usuario {
private:

    int id;
    QString nombreUsuario;
    QString NombreReal;
    QString contrasena;
    QDate fechaRegistro;
    QDate fechaNacimiento;
    QString generoFavorito;
    QString rutaFoto;
    QString CorreoElectronico;
    bool esAdmin;
    bool activo;

public:
    Usuario();
    Usuario(int id, const QString &NombreReal,const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QDate &fecha, const QString &generoFavorito, const QString &rutaFoto, const QString &CorreoElectronico,bool esAdmin, bool activo=true);

    int getId() const;
    QString getNombreReal()const;
    QString getNombreUsuario() const;
    QString getContrasena() const;
    QDate getFechaNacimiento()const;
    QDate getFechaRegistro() const;
    QString getGeneroFavorito() const;
    QString getCorreo()const;
    QString getRutaFoto() const;
    bool getEsAdmin() const;
    bool estaActivo() const;

    void setContrasena(const QString &nueva);
    void desactivar();
};

#endif
