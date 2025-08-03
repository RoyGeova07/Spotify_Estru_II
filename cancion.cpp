#include "cancion.h"

Cancion::Cancion() : idCancion(0),fechaCarga(QDate::currentDate()), activo(true) {}


Cancion::Cancion(int id, const QString &titulo, QString NombreArtista, const QString &genero,const QString &categoria, const QString &tipo, const QString &descripcion,const QString &rutaAudio, const QString &rutaImagen, const QString &duracion,const QDate &fechaCarga, bool activo)
    : idCancion(id), titulo(titulo), NombreArtista(NombreArtista), genero(genero), categoria(categoria),
    tipo(tipo), descripcion(descripcion), rutaAudio(rutaAudio), rutaImagen(rutaImagen),
    duracion(duracion), fechaCarga(fechaCarga), activo(activo) {}


int Cancion::getId()const{return idCancion;}
QString Cancion::getTitulo()const{return titulo;}
QString Cancion::getNombreArtista()const{ return NombreArtista;}
QString Cancion::getGenero()const{ return genero;}
QString Cancion::getCategoria()const{ return categoria;}
QString Cancion::getTipo() const{return tipo;}
QString Cancion::getDescripcion()const{ return descripcion;}
QString Cancion::getRutaAudio()const{ return rutaAudio;}
QString Cancion::getRutaImagen()const{return rutaImagen;}
QString Cancion::getDuracion()const{return duracion;}
QDate Cancion::getFechaCarga() const{return fechaCarga;}
bool Cancion::estaActiva()const {return activo; }

void Cancion::desactivar(){activo =false;}
