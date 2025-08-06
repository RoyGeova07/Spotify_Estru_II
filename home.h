#ifndef HOME_H
#define HOME_H

#include <QDialog>
#include"usuario.h"
#include"artista.h"
#include"reproductormusica.h"

class Home : public QWidget
{
    Q_OBJECT

public:

    explicit Home(const Usuario& usuarioActivo,QWidget*parent=nullptr);

private slots:

    void Regresar();

private:

    //ReproductorMusica*ventanaReproductor=nullptr;
    Usuario usuario;

};

#endif // HOME_H
