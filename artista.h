#ifndef ARTISTA_H
#define ARTISTA_H


#include<QString>
#include<QDate>

class Artista
{

private:

    int idArtista; // Igual al id del Usuario
    QString nombreArtistico;
    QString nombreReal;
    QString pais;
    QString generoMusical;
    QString biografia;
    QString contrasena;
    QDate fechaRegistro;
    QString rutaImagen;
    bool activo;
    QDate fechaNacimiento;

public:
    Artista();
    Artista(int idArtista, const QString &nombreArtistico, const QString &nombreReal,const QString &pais, const QString &generoMusical, const QString &biografia,const QString &contrasena, const QString &rutaImagen, bool activo = true,const QDate &fechaNacimiento=QDate());

    int getId() const;
    QString getNombreArtistico() const;
    QString getNombreReal() const;
    QString getPais() const;
    QString getGenero() const;
    QString getBiografia() const;
    QString getContrasena() const;
    QDate getFechaRegistro() const;
    QString getRutaImagen() const;
    bool estaActivo() const;
    QDate getFechaNacimiento()const;

    void desactivar();

};

#endif // ARTISTA_H
