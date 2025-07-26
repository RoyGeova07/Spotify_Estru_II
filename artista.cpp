#include "artista.h"

Artista::Artista(): idArtista(0), activo(true), fechaRegistro(QDate::currentDate()) {}

Artista::Artista(int idArtista, const QString &nombreArtistico, const QString &nombreReal,const QString &pais, const QString &generoMusical, const QString &biografia,const QString &contrasena, const QString &rutaImagen, bool activo): idArtista(idArtista), nombreArtistico(nombreArtistico), nombreReal(nombreReal),pais(pais), generoMusical(generoMusical), biografia(biografia), contrasena(contrasena),fechaRegistro(QDate::currentDate()), rutaImagen(rutaImagen), activo(activo) {}


int Artista::getId()const{return idArtista;}
QString Artista::getNombreArtistico()const{return nombreArtistico;}
QString Artista::getNombreReal()const{return nombreReal;}
QString Artista::getPais()const{return pais;}
QString Artista::getGenero()const{return generoMusical;}
QString Artista::getBiografia()const{return biografia;}
QString Artista::getContrasena()const{return contrasena;}
QDate Artista::getFechaRegistro()const{return fechaRegistro;}
QString Artista::getRutaImagen()const{return rutaImagen;}
bool Artista::estaActivo()const{return activo;}

void Artista::desactivar(){activo=false;}
