#ifndef MENUINICIO_H
#define MENUINICIO_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class MenuInicio : public QWidget {
    Q_OBJECT

public:
    MenuInicio(QWidget*parent=nullptr);

private slots:

    void abrirRegistroUsuario();
    void abrirRegistroArtista();
    void abrirLogin();
    void salir();

private:
    QLabel*lblTitulo;
    QPushButton*btnRegistrarUsuario;
    QPushButton*btnRegistrarArtista;
    QPushButton*btnIniciarSesion;
    QPushButton*btnSalir;
};

#endif // MENUINICIO_H
