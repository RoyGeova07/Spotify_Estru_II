#include "gestorreproduccion.h"
#include<algorithm>

GestorReproduccion::GestorReproduccion(const QString&ruta):rutaArchivo(ruta){}

//IDEA GENERAL:
/*

    Guardar eventos de reproduccion (un “renglon” por cada vez que alguien escucha algo) en un archivo binario.
    Campos por evento:

    userId (quien escucho)

    songId (que cancion)

    epochMs (cuando ocurrio, en milisegundos desde 1970-01-01)

    msPlayed (cuanto tiempo efectivamente escucho)

    durMs (duracion total de la cancion)

    Las metricas (totales, tops, etc.) se calculan leyendo todos los eventos y agregando en memoria.

    No se guardan contadores dentro de Usuario -> evitas desincronizaciones.


    Esta funcion: Abre el archivo en WriteOnly|Append => no sobreescribe, solo agrega al final.

    Crea QDataStream para escribir binario (rapido y compacto).

    setVersion(Qt_5_15) fija una versiin para mantener compatibilidad binaria entre builds.

    Ejemplo: Usuario 101 escucho la cancion 55; la cancion dura 180 000 ms (3 min); el usuario oyó 95 000 ms;
    hora actual 1 700 000 000 000 ms..

*/

bool GestorReproduccion::registrar(quint32 userId, quint32 songId, quint32 msPlayed, quint32 durMs, qint64 epochMs)
{

    QFile f(rutaArchivo);
    if(!f.open(QIODevice::WriteOnly|QIODevice::Append))return false;
    QDataStream out(&f);
    out.setVersion(QDataStream::Qt_5_15);
    out<<userId<<songId<<epochMs<<msPlayed<<durMs;
    return true;

}
/*

    Esta funcion carga todos los eventos del archivo en memoria (vector out) para procesarlos.

*/
bool GestorReproduccion::leerTodos(QVector<EventoReproduccion> &out)const
{

    QFile f(rutaArchivo);
    if(!f.exists())return true;
    if(!f.open(QIODevice::ReadOnly))return false;
    QDataStream in(&f);
    in.setVersion(QDataStream::Qt_5_15);
    while(!in.atEnd())
    {

        EventoReproduccion ev{};
        in>>ev.userId>>ev.songId>>ev.epochMs>>ev.msPlayed>>ev.durMs;
        out.push_back(ev);

    }
    return true;

}
/*

    Esta funcion define que es una 'escucha valida' (para los rankings)

    usare la regla de spotify, cuenta play si se escucho al menos 30 segundos

*/
bool GestorReproduccion::cuentaComoEscucha(quint32 msPlayed, quint32 durMs)
{


    return msPlayed>=30'000u;

}
/*

    Esta funcion devuelve las metricas personales de un usuario
    se usa hash porque es un diccionario O(1) promedio para contar por cancion
    luego convierte a vector y los ordena por conteo para sacar el top

    EstadisticasUsuario devuelve:
    totalEscuchas: cuantas escuchas validas hizo el usuario.

    totalMs: tiempo total escuchado (ms), incluyendo reproducciones parciales.

    top: (songId, conteo) de sus canciones mas escuchadas, ordenado y recortado a topN.

    Ejemplo de calculo (paso a paso):
    Supon que el archivo tiene estos 4 eventos (formato: user, song, msPlayed, durMs):

    (101, 55, 95 000, 180 000) -> valida (>= 30 000)

    (101, 55, 15 000, 180 000) -> NO valida

    (101, 12, 25 000, 40 000) -> valida (>= 20 000)

    (202, 55, 200 000, 200 000) -> pertenece a otro usuario


    Para statsUsuario(101, topN=5):

    totalMs = 95 000 + 15 000 + 25 000 = 135 000 ms

    totalEscuchas = validas = evento 1 + evento 3 = 2

    porCancion: { 55 -> 1, 12 -> 1 } -> top = [(55,1), (12,1)]

*/
EstadisticasUsuario GestorReproduccion::statsUsuario(quint32 userId, int topN) const
{

    QVector<EventoReproduccion>all;leerTodos(all);
    QHash<quint32,int> porCancion;//songId -> conteo
    EstadisticasUsuario st;// {totalEscuchas,totalMs,top}

    for(const auto& ev : all)
    {

        if(ev.userId!=userId)continue;//filtra por usuario
        st.totalMs+=ev.msPlayed;//suma tiempo (aunque no sea valida)
        if(cuentaComoEscucha(ev.msPlayed, ev.durMs))
        {

            porCancion[ev.songId]+=1;//cuenta 1 escucha valida por cancion
            st.totalEscuchas++;

        }

    }
    //convertir hash a vector y ordenar por conteo
    QVector<QPair<quint32,int>>v;v.reserve(porCancion.size());
    for(auto it=porCancion.begin(); it!=porCancion.end(); ++it)v.push_back({it.key(),it.value()});
    std::sort(v.begin(),v.end(),[](auto a,auto b){return a.second>b.second;});
    if(v.size()>topN)v.resize(topN);
    st.top=v;
    return st;

}
/*

    Esta funcion devuelve los ranking globales
    topCanciones: cuenta solo escuchas validas por songId.

    usuariosMasActivos: cuenta eventos por userId (actividad general).

    Ambos se ordenan por conteo y se recortan a topN.


    Ejemplo: con los 4 eventos anteriores:

    porCancion (validas): (101,55,95 000,180 000) -> +1 a 55; (101,12,25 000,40 000) -> +1 a 12; (202,55,200 000,200 000) -> +1 a 55.
    -> topCanciones = [(55,2), (12,1)]

    porUsuario (eventos): 101 tiene 3, 202 tiene 1 ⇒ usuariosMasActivos = [(101,3), (202,1)]

*/
EstadisticasGlobales GestorReproduccion::statsGlobales(int topN) const
{

    QVector<EventoReproduccion> all;leerTodos(all);
    QHash<quint32,int>porCancion,porUsuario;

    for(const auto& ev : all)
    {

        const bool valida=cuentaComoEscucha(ev.msPlayed,ev.durMs);
        if(valida)
        {

            porCancion[ev.songId]+=1;//para ranking global de canciones validas
            porUsuario[ev.userId]+=1; // actividad por usuario (eventos totales)

        }

    }

    auto ordenarTop=[&](const QHash<quint32,int>&h)
    {

        QVector<QPair<quint32,int>>v; v.reserve(h.size());
        for(auto it=h.begin(); it!=h.end(); ++it) v.push_back({it.key(), it.value()});
        std::sort(v.begin(),v.end(),[](auto a,auto b){return a.second>b.second;});
        if(v.size()>topN)v.resize(topN);
        return v;

    };

    EstadisticasGlobales g;
    g.topCanciones=ordenarTop(porCancion);
    g.usuariosMasActivos=ordenarTop(porUsuario);
    return g;

}
