#include "controlreproduccion.h"
#include<QRandomGenerator>
#include<QFile>

// Que hace SalidaAudio?
//Es un objeto que representa a donde se debe enviar el sonido (parlantes, audífonos, HDMI, etc.).
ControlReproduccion::ControlReproduccion(QObject*parent):QObject(parent),reproductor(new QMediaPlayer(this)),salidaAudio(new QAudioOutput(this)),indiceActual(0),aleatorio(false),repetir(false)
{

    reproductor->setAudioOutput(salidaAudio);//Envia el sonido del QMediaPlayer a salidaAudio, que sera por defecto los parlantes del sistema
    salidaAudio->setVolume(1.0);

    //Este connect escucha cuando la cancion termina, y llama a una funcion para pasar a la siguiente
    //o repetirla si esta activado.
    connect(reproductor,&QMediaPlayer::mediaStatusChanged,this,[=](QMediaPlayer::MediaStatus status){

        if(status==QMediaPlayer::EndOfMedia)
        {

            ManejarFinalCancion();

        }

    });

}

void ControlReproduccion::setListaCanciones(const QVector<Cancion> &canciones)
{

    listaCanciones=canciones;

}

void ControlReproduccion::reproducir(int indice)
{

    if(indice<0||indice>=listaCanciones.size())return;//Verifica que el indice sea valido.

    QString ruta=listaCanciones[indice].getRutaAudio();

    //SI YA ESTA EN ESE INDICE, SIMPLEMENTE SE REANUDA SI ESTA EN PAUSA
    if(indice==indiceActual&&reproductor->playbackState()==QMediaPlayer::PausedState)
    {

       reproductor->play();

    }

    //SI YA SE ESTA REPRODUCIENDO ESA MISMA CANCION, NO SE HACE NADA
    if(indice==indiceActual&&reproductor->playbackState()==QMediaPlayer::PlayingState)
    {

        return;

    }

    indiceActual=indice;

    //DEBUGUEARRRRRRR
    qDebug()<<"Reproduciendo: "<<ruta;
    qDebug()<<"Existe archivo? "<<QFile::exists(ruta);

    if(QFile::exists(ruta))
    {

        reproductor->setSource(QUrl::fromLocalFile(ruta));
        reproductor->play();//AQUI COMIENZA LA REPRODUCCION
        emit indiceActualizado(indiceActual);//INFORMA A LA INTERFAZ

    }

}

void ControlReproduccion::pausar()
{

    reproductor->pause();

}

void ControlReproduccion::reiniciar()
{

    reproductor->setPosition(0);
    reproductor->play();

}

void ControlReproduccion::siguiente()
{

    int siguienteIndice=obtenerSiguienteIndice();
    reproducir(siguienteIndice);

}

void ControlReproduccion::anterior()
{

    int nuevoIndice=(indiceActual-1+listaCanciones.size())%listaCanciones.size();
    reproducir(nuevoIndice);

}

void ControlReproduccion::activarAleatorio(bool activar)
{

    aleatorio=activar;

}

void ControlReproduccion::activarRepetir(bool activar)
{

    repetir=activar;

}

int ControlReproduccion::obtenerSiguienteIndice()
{

    if(repetir)return indiceActual;

    if(aleatorio)
        return QRandomGenerator::global()->bounded(listaCanciones.size());

    return (indiceActual+1)%listaCanciones.size();

}

void ControlReproduccion::ManejarFinalCancion()
{

    siguiente();//CUANDO LLEGA AL FINAL, PASA A LA SIGUIENTE

}

int ControlReproduccion::getIndiceActual()const
{

    return indiceActual;

}

QMediaPlayer*ControlReproduccion::getReproductor()const
{

    return reproductor;

}

void ControlReproduccion::play()
{

    //SI YA SE ESTA REPRODUCIENDO NO SE HACE NADA
    if(reproductor->playbackState()==QMediaPlayer::PlayingState)
        return;

    //SI ESTA PAUSADO O DETENIDO, SE REANUDA
    reproductor->play();

}

void ControlReproduccion::detener()
{

    reproductor->stop();

}
