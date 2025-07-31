#include "usuario.h"

Usuario::Usuario(): id(0), fechaRegistro(QDate::currentDate()), esAdmin(false), activo(true) {}

Usuario::Usuario(int id, const QString &nombreUsuario, const QString &contrasena,const QDate &fechaNacimiento,const QDate &fecha,const QString &generoFavorito, const QString &rutaFoto,bool esAdmin, bool activo): id(id), nombreUsuario(nombreUsuario), contrasena(contrasena),fechaRegistro(fecha), fechaNacimiento(fechaNacimiento),generoFavorito(generoFavorito), rutaFoto(rutaFoto),esAdmin(esAdmin), activo(activo) {}

int Usuario::getId() const { return id; }
QString Usuario::getNombreUsuario() const { return nombreUsuario; }
QString Usuario::getContrasena() const { return contrasena; }
QDate Usuario::getFechaRegistro() const { return fechaRegistro; }
QString Usuario::getGeneroFavorito() const { return generoFavorito; }
QString Usuario::getRutaFoto() const { return rutaFoto; }
bool Usuario::getEsAdmin() const { return esAdmin; }
bool Usuario::estaActivo() const { return activo; }

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
