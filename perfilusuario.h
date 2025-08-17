#ifndef PERFILUSUARIO_H
#define PERFILUSUARIO_H

#include<QWidget>
#include<QLabel>
#include<QTextEdit>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QGridLayout>
#include<QPainter>
#include<QPainterPath>
#include"usuario.h"
#include<QTableWidget>
#include<QScrollArea>
#include<QHeaderView>

class PerfilUsuario:public QWidget
{

    Q_OBJECT

public:
    explicit PerfilUsuario(const Usuario&u,QWidget*parent=nullptr);


private:
    Usuario usuario;

    QLabel*foto=nullptr;
    QLabel*lblNombreUsuario=nullptr;
    QLabel*lblNombreReal=nullptr;
    QLabel*lblCorreo=nullptr;
    QLabel*lblGeneroFav=nullptr;
    QLabel*lblFechaReg=nullptr;
    QLabel*lblFechaNac=nullptr;
    QLabel*lblRol=nullptr;
    QLabel*lblEstado=nullptr;
    QLabel*lblId=nullptr;

    void cargarFotoCircular(const QString& ruta,const QSize& size);
    QWidget* crearTarjeta(const QString& titulo,const QString& valor,const QString& nota=QString());
    QTableWidget* crearTabla(const QStringList& cabeceras,int filasDemo=0);

    QWidget* cardEscuchas=nullptr;
    QWidget* cardTiempo=nullptr;
    QTableWidget* tablaTop=nullptr;
    QWidget*cardPromedio=nullptr;

    void cargarEstadisticas();
    static QString aHMS(quint64 ms)
    {

        quint64 s=ms/1000;
        quint64 h=s/3600; s%=3600;
        quint64 m=s/60;   s%=60;
        return QString("%1:%2:%3").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0'));

    }

    void VolverHome();

};

#endif // PERFILUSUARIO_H
