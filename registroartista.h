#ifndef REGISTROARTISTA_H
#define REGISTROARTISTA_H

#include<QDialog>
#include<QLineEdit>
#include<QLabel>
#include<QTextEdit>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include <QComboBox>
#include "gestorUsuarios.h"
#include "gestorArtistas.h"
#include<QDateEdit>
#include<QScrollArea>

class RegistroArtista:public QDialog{

    Q_OBJECT

public:
    RegistroArtista(QWidget*parent=nullptr);

private slots:

    void RegistrarArtista();
    void RegresarAlMenu();

private:

    QLineEdit*txtContrasena;
    QLineEdit*txtNombreArtistico;
    QLineEdit*txtNombreReal;
    QComboBox*comboPais;
    QComboBox*comboGeneroMusical;
    QTextEdit*txtBiografia;
    QLineEdit*txtRutaImagen;
    QLabel*lblResultado;
    QPushButton*btnRegistrar;
    QPushButton*btnSeleccionarImagen;
    QLabel*vistaPreviaImagen;
    QPushButton* btnVolver;
    QDateEdit*dateNacimiento;
    QLabel*lblIdArtista;

    GestorUsuarios gestorUsuarios;
    GestorArtistas gestorArtistas;

};

#endif // REGISTROARTISTA_H
