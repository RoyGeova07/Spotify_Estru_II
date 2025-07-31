#ifndef USUARIO_H
#define USUARIO_H

#include <QString>
#include <QDate>

class Usuario {
private:
    int id;
    QString nombreUsuario;
    QString contrasena;
    QDate fechaRegistro;
    QDate fechaNacimiento;
    QString generoFavorito;
    QString rutaFoto;
    bool esAdmin;
    bool activo;

public:
    Usuario();
    Usuario(int id, const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QDate &fecha, const QString &generoFavorito, const QString &rutaFoto, bool esAdmin, bool activo=true);

    int getId() const;
    QString getNombreUsuario() const;
    QString getContrasena() const;
    QDate getFechaNacimiento()const;
    QDate getFechaRegistro() const;
    QString getGeneroFavorito() const;
    QString getRutaFoto() const;
    bool getEsAdmin() const;
    bool estaActivo() const;

    void setContrasena(const QString &nueva);
    void desactivar();
};

#endif
