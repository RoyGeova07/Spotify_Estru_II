#include "home.h"
#include <QLabel>
#include <QVBoxLayout>

Home::Home(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Pantalla Principal");
    setFixedSize(400, 300);

    QLabel* mensaje = new QLabel("🎵 ¡Bienvenido a la plataforma musical!", this);
    mensaje->setAlignment(Qt::AlignCenter);
    mensaje->setStyleSheet("font-size: 16px; font-weight: bold; color: #1DB954;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(mensaje);
    setLayout(layout);
}
