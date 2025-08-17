#include "cancion.h"

Cancion::Cancion() : idCancion(0),fechaCarga(QDate::currentDate()), activo(true) {}


Cancion::Cancion(int reproducciones,int id, const QString &titulo, QString NombreArtista,Genero genero,Categoria categoria,Tipo tipo, const QString &descripcion,const QString &rutaAudio, const QString &rutaImagen, const QString &duracion,const QDate &fechaCarga, bool activo)
    : idCancion(id),titulo(titulo), NombreArtista(NombreArtista), genero(genero), categoria(categoria),
    tipo(tipo), descripcion(descripcion), rutaAudio(rutaAudio), rutaImagen(rutaImagen),
    duracion(duracion), fechaCarga(fechaCarga), activo(activo),reproducciones(reproducciones) {}


int Cancion::getId()const{return idCancion;}
QString Cancion::getTitulo()const{return titulo;}
QString Cancion::getNombreArtista()const{ return NombreArtista;}
Genero Cancion::getGenero()const{ return genero;}
Categoria Cancion::getCategoria()const{ return categoria;}
Tipo Cancion::getTipo() const{return tipo;}
QString Cancion::getDescripcion()const{ return descripcion;}
QString Cancion::getRutaAudio()const{ return rutaAudio;}
QString Cancion::getRutaImagen()const{return rutaImagen;}
QString Cancion::getDuracion()const{return duracion;}
QDate Cancion::getFechaCarga() const{return fechaCarga;}
bool Cancion::estaActiva()const {return activo; }
int Cancion::getReproducciones()const{return reproducciones;}
void Cancion::setReproducciones(int r){reproducciones=r;}
void Cancion::incrementarReproducciones()
{

    reproducciones++;

}

void Cancion::desactivar(){activo =false;}

void Cancion::setTitulo(const QString &t){titulo=t;}
void Cancion::setCategoria(Categoria c){categoria=c;}
void Cancion::setDescripcion(const QString &d){descripcion=d;}
void Cancion::setGenero(Genero g){genero=g;}
void Cancion::setCaratula(const QString &cara){rutaImagen=cara;}


void Cancion::setRutaAudio(const QString &r) {
    rutaAudio = r;
}

void Cancion::setRutaImagen(const QString &r) {
    rutaImagen = r;
}

void Cancion::setDuracion(const QString &d) {
    duracion = d;
}

void Cancion::setFechaCarga(const QDate &f) {
    fechaCarga = f;
}

void Cancion::setActiva(bool a) {
    activo = a;
}

void Cancion::setNombreArtista(const QString &a) {
    NombreArtista = a;
}

void Cancion::setTipo(Tipo t) {
    tipo = t;
}

void Cancion::setID(int i)
{

    idCancion=i;

}
