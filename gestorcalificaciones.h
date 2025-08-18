#ifndef GESTORCALIFICACIONES_H
#define GESTORCALIFICACIONES_H

#include<QVector>
#include<QFile>
#include<QDataStream>
#include<QDateTime>
#include<QHash>
#include<QtGlobal>

struct RegistroCalificacion
{

    quint32 userId;
    quint32 songId;
    quint8 rating;   // 1..5
    qint64 epochMs;  // ms desde 1970

};

struct StatsUsuarioRate
{

    double promedioOtorgado=0.0;
    int cantidad=0;
    QVector<RegistroCalificacion>ultimas; //orden cronologico (mas recientes al final)

};

class GestorCalificaciones
{
public:

    explicit GestorCalificaciones(const QString&ruta="calificaciones.dat"):rutaArchivo(ruta){}

    /*

        esta funcion escribe una calificacion (si no existe una previa del mismo usuario/cancion)
        devuelve false si ya habia calificado o si falla

    */
    bool registrar(quint32 userID,quint32 songId,quint8 rating,quint64 epochMs=QDateTime::currentMSecsSinceEpoch());

    /*

        me sirve para el:
        el usuario ya califico esta cancion?

    */
    bool yaCalifico(quint32 userId,quint32 songId,quint8* outRating=nullptr)const;
    /*

        funcion para cargar todo en memoria

    */
    bool leerTodas(QVector<RegistroCalificacion>&out)const;
    /*

        funcion para hacer el promedio y conteo por cancion (avg=0 y count=0 si no hay datos)

    */
    bool promedioCancion(quint32 songId,double&avg,int&count)const;
    /*

        funcion para hacer la distribucion 1.... al 5 para una cancion (vector de 5 enteros)

    */
    QVector<int>distribucionPorCancion(quint32 songId)const;
    /*

        Top por promedio (desc), filtrando por min. de calificaciones para estabilidad

    */
    QVector<QPair<quint32,double>> topCancionesMejorCalif(int topN=10,int minCalifs=5)const;
    /*

        usuarios que mas califican

    */
    QVector<QPair<quint32,int>> usuariosQueMasCalifican(int topN=10) const;
    /*

        Resumen por usuario (promedio otorgado, conteo, ultimas N)

    */
    StatsUsuarioRate statsUsuario(quint32 userId,int ultimasN=10)const;

private:

    QString rutaArchivo;

    static constexpr quint32 MAGIC= 0x52415445;
    static constexpr quint16 VERSION=1;

};

#endif // GESTORCALIFICACIONES_H
