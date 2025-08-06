#ifndef CANCION_H
#define CANCION_H

#include<QString>
#include<QDate>
#include"tipos.h"

class Cancion
{

private:

    int idCancion;
    QString titulo;
    QString NombreArtista;
    Genero genero;
    Categoria categoria;//vacio, hasta q el usuario lo modifique
    Tipo tipo; // single, EP, album
    QString descripcion;
    QString rutaAudio;//mp3 o mp4
    QString rutaImagen;
    QString duracion; // RECORDAR: usar QString para facilitar el formato hh:mm:ss
    QDate fechaCarga;
    bool activo;
    int reproducciones;

public:
    Cancion();

    Cancion(int reproducciones,int id, const QString &titulo, QString NombreArtista,Genero genero,Categoria categoria,Tipo tipo, const QString &descripcion, const QString &rutaAudio,const QString &rutaImagen, const QString &duracion, const QDate &fechaCarga,bool activo=true);

    // Getters
    int getId() const;
    QString getTitulo() const;
    QString getNombreArtista() const;
    Genero getGenero() const;
    Categoria getCategoria() const;
    Tipo getTipo() const;
    QString getDescripcion() const;
    QString getRutaAudio() const;
    QString getRutaImagen() const;
    QString getDuracion() const;
    QDate getFechaCarga() const;
    bool estaActiva() const;
    int getReproducciones()const;
    void setReproducciones(int r);
    void incrementarReproducciones();

    // Logica
    void desactivar();


};

#endif // CANCION_H
