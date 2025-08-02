#ifndef PANTALLAADMIN_H
#define PANTALLAADMIN_H

#include<QWidget>
#include<QComboBox>
#include<QTextEdit>
#include<QPushButton>
#include<QLabel>
#include<QString>
#include"artista.h"

class PantallaAdmin : public QWidget
{
    Q_OBJECT

public:

    explicit PantallaAdmin(const Artista &artistaActivo, QWidget *parent = nullptr);

private slots:

    void seleccionarArchivoAudio();
    void seleccionarPortada();
    void subirCancion();

private:

    Artista artista;  // Artista logueado

    QLabel *lblTitulo;
    QLabel *lblDuracion;
    QLabel *lblAudioSeleccionado;
    QLabel *lblPortadaSeleccionada;
    QLabel *lblVistaPreviaPortada;

    QComboBox *comboGenero;
    QComboBox *comboCategoria;
    QComboBox *comboTipo;
    QTextEdit *txtDescripcion;

    QPushButton *btnSeleccionarAudio;
    QPushButton *btnSeleccionarPortada;
    QPushButton *btnSubir;

    QString rutaAudio;
    QString rutaImagen;
    QString titulo;
    QString duracion;

    void configurarUI();
};

#endif // PANTALLAADMIN_H
