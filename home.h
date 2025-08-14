#ifndef HOME_H
#define HOME_H

#include<QWidget>
#include"usuario.h"
#include"artista.h"
#include"reproductormusica.h"

class QListWidget;
class QLabel;//PARA COMPILACION MAS RAPIDA

class Home : public QWidget
{
    Q_OBJECT

public:

    explicit Home(const Usuario& usuarioActivo,QWidget*parent=nullptr);

private slots:

    void Regresar();
    void AbrirPorGenero(Genero g);

private:

    Usuario usuario;
    QListWidget*listaPlaylists=nullptr;
    QLabel*lblVacio=nullptr;

    void cargarPlaylists();
    void crearPlaylist();

};
#endif // HOME_H
