#ifndef GESTORREPRODUCCION_H
#define GESTORREPRODUCCION_H


#include<QVector>
#include<QDateTime>
#include<QFile>
#include<QDataStream>
#include<QHash>

/*

    qint32 -> entero con signo de 32 bits
    quint32 -> entero sin signo de 32 bits
    qint64 -> entero con signo de 64 bits

    la diferencia con int aa secas es que int no garantiza tamaño fijo en C++
    (suele ser 32 bits hoy, pero no es parte del estandar). Con los tipos de Qt, si garantizas que ocupan exactamente
    4 u 8 bytes siempre. Eso es clave cuando:

    Cuando se escribe o se lee en binario con QDataStream se necesita tamaños fijos para que el archivo sea portable)

    Haces calculos grandes (tiempos, acumulados) y no se requiere overflow osea tiempos y acumulados en 64 bits -> no se rompen al crecer.

    y se usara hash para Para contar cuantas veces aparecio cada cancion/usuario” usamos un diccionario:

    QHash<quint32,int> porCancion; -> O(1) promedio en insertar/consultar.

    Con QMap sería O(log n) y no necesitamos orden por clave.

    Despues pasamos el hash a un QVector<QPair<id,conteo>> y ordenamos por conteo DESC
    con std::sort, para obtener el ranking.


    por ende se usa qint64 donde los numeros pueden crecer muchisimo (fechas y totales).

    se usa qint32 donde los valores son no negativos y seguros en 32 bits (IDs, duraciones por evento).s

    y con eso se gana seguridad (sin desbordes), portabilidad del archivo, y claridad en el código
    y eso es exactamente lo que necesito para que las estadisticas sean fiables.

*/

struct EventoReproduccion
{

    quint32 userId;
    quint32 songId;
    qint64  epochMs;// inicio del evento (UTC, ms)
    quint32 msPlayed;// ms escuchados
    quint32 durMs;// duracion de la canción (ms)

};

struct EstadisticasUsuario
{

    int totalEscuchas=0;
    qint64 totalMs=0;//tiempo total (ms)
    QVector<QPair<quint32,int>>top;//(songId, conteo) ordenado desc

};

struct EstadisticasGlobales
{

    QVector<QPair<quint32,int>> topCanciones;
    QVector<QPair<quint32,int>> usuariosMasActivos;

};

class GestorReproduccion
{
public:

    explicit GestorReproduccion(const QString&ruta="reproducciones.dat");

    //Guarda un evento (llámar al terminar una cancion o al cambiar de pista)
    bool registrar(quint32 userId,quint32 songId,quint32 msPlayed,quint32 durMs,qint64 epochMs=QDateTime::currentMSecsSinceEpoch());

    // Reglas: cuenta como “escucha” si escucho >= min(30s, 50% de la cancion)
    static bool cuentaComoEscucha(quint32 msPlayed, quint32 durMs);

    // Lecturas para usuario / admin
    EstadisticasUsuario statsUsuario(quint32 userId,int topN=5)const;
    EstadisticasGlobales statsGlobales(int topN=10)const;
    bool leerTodos(QVector<EventoReproduccion>&out)const;


private:

    QString rutaArchivo;

};

#endif // GESTORREPRODUCCION_H
