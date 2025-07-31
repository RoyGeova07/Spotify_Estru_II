#ifndef CANCION_H
#define CANCION_H

#include<QString>
#include<QDate>

class Cancion
{

private:

    int idCancion;
    QString titulo;
    int idArtista; // Referencia al artista que la subio
    QString genero;
    QString categoria;//vacio, hasta q el usuario lo modifique
    QString tipo; // single, EP, album
    QString descripcion;
    QString rutaAudio;//mp3 o mp4
    QString rutaImagen;
    QString duracion; // RECORDAR: usar QString para facilitar el formato hh:mm:ss
    QDate fechaCarga;
    bool activo;

public:
    Cancion();

    Cancion(int id, const QString &titulo, int idArtista, const QString &genero, const QString &categoria,const QString &tipo, const QString &descripcion, const QString &rutaAudio,const QString &rutaImagen, const QString &duracion, const QDate &fechaCarga,bool activo=true);

    // Getters
    int getId() const;
    QString getTitulo() const;
    int getIdArtista() const;
    QString getGenero() const;
    QString getCategoria() const;
    QString getTipo() const;
    QString getDescripcion() const;
    QString getRutaAudio() const;
    QString getRutaImagen() const;
    QString getDuracion() const;
    QDate getFechaCarga() const;
    bool estaActiva() const;

    // Logica
    void desactivar();


};

#endif // CANCION_H
