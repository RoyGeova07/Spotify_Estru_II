#ifndef VISITAPERFILARTISTA_H
#define VISITAPERFILARTISTA_H

#include<QWidget>
#include<QLabel>
#include<QScrollArea>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QPushButton>
#include<QMap>
#include"artista.h"
#include"cancion.h"
#include"gestorcanciones.h"
#include"reproductormusica.h"
#include"usuario.h"

class VisitaPerfilArtista:public QWidget
{
    Q_OBJECT

public:

    explicit VisitaPerfilArtista(const Artista& artista,const Usuario& usuarioActual,QWidget*parent=nullptr);

private:
    // Datos
    Artista artista;
    Usuario usuario;

    QVBoxLayout*root=nullptr;

    // Encabezado
    QLabel*lblFoto=nullptr;
    QLabel*lblNombre=nullptr;
    QLabel*lblPais=nullptr;
    QLabel*lblRegistro=nullptr;
    QLabel*lblGeneroMusical=nullptr;
    QLabel*lblNacimiento=nullptr;
    QLabel*lblDescripcion=nullptr;

    QWidget* crearFilaSingles(const QVector<Cancion>& canciones, const QString& tituloFila);
    QPixmap circularPixmap(const QPixmap& src, const QSize& size) const;

    void construirUI();
    void cargarCancionesDelArtista(QVector<Cancion>& singles,QMap<QString, QVector<Cancion>>& eps,QMap<QString, QVector<Cancion>>& albums);
    void VolverHome();

};

#endif // VISITAPERFILARTISTA_H

