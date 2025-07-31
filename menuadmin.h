#ifndef MENUADMIN_H
#define MENUADMIN_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include "artista.h"

class MenuAdmin : public QWidget
{
    Q_OBJECT

public:

    explicit MenuAdmin(const Artista& artistaActivo,QWidget*parent=nullptr);

private slots:

    void abrirVentanaSubirCancion();
    //void abrirMiMusica();
    void CerrarSesion();

private:

    Artista artista;
    QLabel*lblFoto;
    QLabel*lblNombre;
    QPushButton*btnSubirCancion;
    QPushButton*btnEditarCancion;
    QPushButton*btnEliminarCancion;
    QPushButton*btnVerEstadisticas;
    QPushButton*btnSalir;
    QPushButton*btnMiMusica;

    void configurarUI();
    void estiloBoton(QPushButton* boton, const QString& color);

};

#endif // MENUADMIN_H
