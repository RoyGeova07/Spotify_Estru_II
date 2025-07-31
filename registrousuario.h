#ifndef REGISTROUSUARIO_H
#define REGISTROUSUARIO_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include<QDateEdit>
#include "gestorUsuarios.h"

class RegistroUsuario : public QDialog {
    Q_OBJECT

public:
    RegistroUsuario(QWidget*parent=nullptr);

private slots:

    void RegistrarUsuario();
    void SeleccionarImagen();
    void RegresarAlMenu();

private:

    QLineEdit*txtNombreUsuario;
    QLineEdit*txtContrasena;
    QComboBox*comboPais;
    QComboBox*comboGeneroMusical;
    QLineEdit*txtRutaImagen;
    QLabel*vistaPreviaImagen;
    QPushButton*btnSeleccionarImagen;
    QPushButton*btnRegistrar;
    QPushButton*btnVolver;
    QLabel*lblResultado;
    QDateEdit*dateNacimiento;

    GestorUsuarios gestorUsuarios;
};

#endif // REGISTROUSUARIO_H
