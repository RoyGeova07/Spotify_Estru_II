#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class LogIn : public QDialog {
    Q_OBJECT

public:

    LogIn(QWidget*parent=nullptr);

private:

    QLineEdit*txtUsuario;
    QLineEdit*txtContrasena;
    QPushButton*btnIniciar,*btnVolver;

private slots:
    void validarCredenciales();
    void volverr();

};

#endif // LOGIN_H
