#include "usuario.h"

Usuario::Usuario(): id(0), fechaRegistro(QDate::currentDate()), esAdmin(false), activo(true) {}

Usuario::Usuario(int id,const QString &NombreReal, const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QDate &fecha,const QString &generoFavorito, const QString &rutaFoto,const QString &CorreoElectronico,bool esAdmin, bool activo): id(id), NombreReal(NombreReal),nombreUsuario(nombreUsuario), contrasena(contrasena),fechaRegistro(fecha), fechaNacimiento(fechaNacimiento),generoFavorito(generoFavorito), rutaFoto(rutaFoto),CorreoElectronico(CorreoElectronico),esAdmin(esAdmin),activo(activo) {}

int Usuario::getId() const { return id; }
QString Usuario::getNombreUsuario() const { return nombreUsuario; }
QString Usuario::getContrasena() const { return contrasena; }
QDate Usuario::getFechaRegistro() const { return fechaRegistro; }
QString Usuario::getGeneroFavorito() const { return generoFavorito; }
QString Usuario::getRutaFoto() const { return rutaFoto; }
bool Usuario::getEsAdmin() const { return esAdmin; }
bool Usuario::estaActivo() const { return activo; }
QString Usuario::getNombreReal()const{return NombreReal;}

void Usuario::setContrasena(const QString &nueva) {
    contrasena = nueva;
}
void Usuario::desactivar() {
    activo = false;
}

QDate Usuario::getFechaNacimiento()const
{

    return fechaNacimiento;

}

QString Usuario::getCorreo()const
{

    return CorreoElectronico;

}
