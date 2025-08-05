#ifndef MENUADMIN_H
#define MENUADMIN_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include "artista.h"
#include<QLineEdit>
#include<QTextEdit>
#include<QComboBox>
#include<QTabWidget>
#include<QVector>
#include"vistapreviaimagen.h"

class MenuAdmin : public QWidget
{
    Q_OBJECT

public:

    explicit MenuAdmin(const Artista& artistaActivo,QWidget*parent=nullptr);

private slots:

    void abrirVentanaSubirCancion();
    //void abrirMiMusica();
    void CerrarSesion();

    void seleccionarAudio();
    void tipoSeleccionado(const QString& tipo);
    void subirCanciones();
    void SeleccionarImagen();

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

    //=========================================================
    QLabel*lblNombreArtista;
    QComboBox*comboTipo;  // Single, EP, Álbum
    QTabWidget*tabsCanciones;
    QPushButton*btnSubir;

    struct WidgetCancion
    {

        QLabel*lblIdCancion;
        QLineEdit*txtTitulo;
        QComboBox*txtGenero;
        QLineEdit*txtDuracion;
        QComboBox*comboCategoria;
        QTextEdit*txtDescripcion;
        QLineEdit*txtRutaAudio;
        QLineEdit*txtRutaImagen;
        QPushButton*btnSeleccionarAudio;
        QPushButton*btnSeleccionarImagen;
        VistaPreviaImagen* vistaPreviaImagen;

    };


    QVector<WidgetCancion>cancionesWidgets;

    void crearPestañasCanciones(int cantidad);
    QWidget* crearFormularioCancion(int index,int ID);

    QVBoxLayout*layoutDerecho;

};

#endif // MENUADMIN_H
