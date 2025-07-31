#ifndef SUBIR_CANCION_H
#define SUBIR_CANCION_H

#include<QWidget>
#include<QLineEdit>
#include<QTextEdit>
#include<QComboBox>
#include<QPushButton>
#include<QTabWidget>
#include<QLabel>
#include<QVector>
#include"artista.h"
#include"vistapreviaimagen.h"

class Subir_Cancion:public QWidget
{

    Q_OBJECT

public:

    Subir_Cancion(const Artista& artistaActivo,QWidget*parent=nullptr);

private slots:

    void seleccionarAudio();
    void tipoSeleccionado(const QString& tipo);
    void subirCanciones();
    void SeleccionarImagen();
    void volverAlMenu();

private:

    Artista artista;
    QLabel* lblNombreArtista;
    QComboBox* comboTipo;  // Single, EP, Álbum
    QTabWidget* tabsCanciones;
    QPushButton* btnSubir;
    QPushButton*btnVolver;

    struct WidgetCancion
    {

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


    QVector<WidgetCancion> cancionesWidgets;

    void configurarUI();
    void crearPestañasCanciones(int cantidad);
    QWidget* crearFormularioCancion(int index);

};

#endif // SUBIR_CANCION_H
