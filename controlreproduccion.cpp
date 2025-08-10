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

            ManejarFinalCancion();//avanza segun reglas (repetir/aleatorio/lista circular)

        }

    });

}

ControlReproduccion::~ControlReproduccion()
{

    limpiarLista();

}

void ControlReproduccion::limpiarLista()
{

    //LIBERAR NODOS CIRCULARES
    for(Nodo*n:mapaIndices)delete n;
    mapaIndices.clear();
    head=nullptr;
    actual=nullptr;

}

//Construye la lista doblemente enlazada circular a partir de listaCanciones
void ControlReproduccion::construirLista()
{

    limpiarLista();// primero limpia cualquier lista anterior

    const int n=listaCanciones.size();

    if(n==0){indiceActual=-1;return;}//nada que construir si no hay canciones

    mapaIndices.resize(n,nullptr);//reserva vector de punteros

    //SE CREAN LOS NODOS
    Nodo*prev=nullptr;
    for(int i=0;i<n;i++)
    {

        Nodo*node=new Nodo{i,nullptr,nullptr};
        mapaIndices[i]=node;//acceso directo index -> nodo

        if(!head)head=node;//si es el primero, head = node
        if(prev)
        {
            //encadenar doblemente
            prev->next=node;
            node->prev=prev;

        }
        prev=node;//AVANZAR EL ULTIMO

    }

    //CERRAR EL CIRCULO:  head <-> prev
    head->prev=prev;
    prev->next=head;

    //POR DEFECTO APUNTAR AL PRIMERO
    actual=head;
    indiceActual=0;

}

//Setter de la lista de canciones. Reconstruye la lista circular.
void ControlReproduccion::setListaCanciones(const QVector<Cancion> &canciones)
{

    listaCanciones=canciones;
    construirLista();// crea nodos/relaciones circulares y coloca actual=0

}


//Salta a un nodo especifico de la lista circular y reproduce.
//Ademas emite la señal indiceActualizado para refrescar la UI.
void ControlReproduccion::irANodo(Nodo *n)
{

    if(!n)return;
    actual=n;//mueve el puntero actual
    indiceActual=n->index;//actualiza indice

    const QString ruta=listaCanciones[indiceActual].getRutaAudio();
    qDebug()<<"REPRODUCIENDO: "<<ruta<<" Existe?"<<QFile::exists(ruta);

    // Solo cargar si el archivo existe fisicamente
    if(QFile::exists(ruta))
    {

        // Cargar la fuente local en el reproductor
        reproductor->setSource(QUrl::fromLocalFile(ruta));
        reproductor->play(); // Empezar a reproducir
        emit indiceActualizado(indiceActual);//Avisar a la interfaz que cambio el indice

    }

}

// Reproduccion por indice.
// Casos:
//  - Si el indice es invalido -> return.
//  - Si es el MISMO indice:
//        * Si el player NO esta en Playing -> recarga y play (fix del "primer click no suena").
//        * Si ya esta sonando -> no hacemos nada.
//  - Si es diferente -> saltar al nodo correspondiente y reproducir.
void ControlReproduccion::reproducir(int indice)
{

    if(indice<0||indice>=listaCanciones.size())return;//Verifica que el indice sea valido.

    //SI YA ESTAMOS EN EL INDICE
    if(actual&&indice==indiceActual)
    {

        //pero NO estA sonando (Paused o Stopped), aseguramos cargar y reproducir
        if(reproductor->playbackState()!=QMediaPlayer::PlayingState)
        {

            irANodo(actual);

        }
        return;//si ya estaba sonando, no hacemos nada

    }
    if(mapaIndices.isEmpty())construirLista();
    irANodo(mapaIndices[indice]); // setSource + play + emite indiceActualizado

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

//Devuelve un nodo aleatorio de la lista (para modo aleatorio)
ControlReproduccion::Nodo* ControlReproduccion::nodoAleatorio()const
{

    if(mapaIndices.isEmpty())return nullptr;
    int idx=QRandomGenerator::global()->bounded(mapaIndices.size());
    return mapaIndices[idx];

}

// Avanza a la siguiente pista.
// Reglas:
//   - repetir ON   -> vuelve a cargar la misma pista actual
//   - aleatorio ON -> elige un nodo aleatorio
//   - normal       -> actual->next (siempre valido por ser circular)
void ControlReproduccion::siguiente()
{

    if(!actual)return;
    if(repetir){irANodo(actual);return;}
    if(aleatorio){irANodo(nodoAleatorio());return;}
    irANodo(actual->next);//circular: nunca se sale de rango

}

void ControlReproduccion::anterior()
{
    if(!actual)return;
    if(repetir){irANodo(actual);return;}
    if(aleatorio){irANodo(nodoAleatorio());return;}
    irANodo(actual->prev);//CIRCULARRR

}

void ControlReproduccion::activarAleatorio(bool activar)
{

    aleatorio=activar;

}

void ControlReproduccion::activarRepetir(bool activar)
{

    repetir=activar;

}


void ControlReproduccion::ManejarFinalCancion()
{

    siguiente();//CUANDO LLEGA AL FINAL, PASA A LA SIGUIENTE

}

int ControlReproduccion::getIndiceActual()const
{

    return indiceActual;

}

// Devuelve el puntero al QMediaPlayer (para conectar señales/consultar estado desde la UI)
QMediaPlayer*ControlReproduccion::getReproductor()const
{

    return reproductor;

}

void ControlReproduccion::play()
{

    if(reproductor->playbackState()!=QMediaPlayer::PlayingState)
    {

        reproductor->play();

    }


}

void ControlReproduccion::detener()
{

    reproductor->stop();

}
