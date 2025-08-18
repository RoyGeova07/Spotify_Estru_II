#include "gestorcalificaciones.h"
#include<algorithm>

bool GestorCalificaciones::registrar(quint32 userID, quint32 songId, quint8 rating, quint64 epochMs)
{

    if(rating<1||rating>5)return false;

    quint8 previo=0;
    if(yaCalifico(userID,songId,&previo))return false;//un sola vez se califica

    QFile f(rutaArchivo);
    if(!f.open(QIODevice::WriteOnly|QIODevice::Append))return false;
    QDataStream out(&f);
    out.setVersion(QDataStream::Qt_5_15);

    //Cabecera por registro para robustez
    out<<MAGIC<<VERSION;
    out<<userID<<songId<<rating<<epochMs;
    return true;

}

bool GestorCalificaciones::yaCalifico(quint32 userId, quint32 songId, quint8 *outRating) const
{

    QFile f(rutaArchivo);
    if(!f.exists())return false;
    if(!f.open(QIODevice::ReadOnly))return false;
    QDataStream in(&f);
    in.setVersion(QDataStream::Qt_5_15);

    while(!in.atEnd())
    {

        quint32 magic=0;quint16 ver=0;
        in>>magic>>ver;
        if(in.status()!=QDataStream::Ok)break;
        RegistroCalificacion r;
        in>>r.userId>>r.songId>>r.rating>>r.epochMs;
        if(magic!=MAGIC)continue; // salta basura o versiones viejas
        if(r.userId==userId&&r.songId==songId)
        {

            if(outRating)*outRating=r.rating;
            return true;

        }

    }
    return false;

}

bool GestorCalificaciones::leerTodas(QVector<RegistroCalificacion> &out) const
{

    out.clear();
    QFile f(rutaArchivo);
    if(!f.exists())return false;
    if(!f.open(QIODevice::ReadOnly))return false;
    QDataStream in(&f);
    in.setVersion(QDataStream::Qt_5_15);

    while(!in.atEnd())
    {

        quint32 magic=0;quint16 ver=0;
        in>>magic>>ver;
        if(in.status()!=QDataStream::Ok)break;
        RegistroCalificacion r;
        in>>r.userId>>r.songId>>r.rating>>r.epochMs;
        if(magic==MAGIC)out.push_back(r);

    }
    return true;

}

bool GestorCalificaciones::promedioCancion(quint32 songId, double &avg, int &count) const
{

    avg=0.0;count=0;
    QVector<RegistroCalificacion>all;
    if(!leerTodas(all))return false;
    long long suma=0;
    for(const auto&r:all)if(r.songId==songId){suma+=r.rating;++count;}
    if(count>0)avg=double(suma)/double(count);
    return true;

}

QVector<int> GestorCalificaciones::distribucionPorCancion(quint32 songId) const
{

    QVector<int> dist(5,0);
    QVector<RegistroCalificacion>all;
    if(!leerTodas(all))return dist;
    for(const auto&r:all)if(r.songId==songId)
    {

        if(r.rating>=1&&r.rating<=5)dist[r.rating-1]+=1;

    }
    return dist;

}
QVector<QPair<quint32,double>> GestorCalificaciones::topCancionesMejorCalif(int topN, int minCalifs) const
{

    QVector<RegistroCalificacion>all;
    leerTodas(all);
    QHash<quint32,QPair<long long,int>>acum;//songId -> (suma,count)
    for(const auto& r:all)
    {

        auto&p=acum[r.songId];
        p.first+=r.rating;
        p.second+=1;

    }
    QVector<QPair<quint32,double>>v;
    v.reserve(acum.size());
    v.reserve(acum.size());
    for(auto it=acum.begin(); it!=acum.end();++it)
    {

        if(it.value().second>=minCalifs)
        {

            v.push_back({it.key(),double(it.value().first)/double(it.value().second)});

        }

    }
    std::sort(v.begin(),v.end(),[](auto a,auto b){return a.second>b.second;});
    if(v.size()>topN)v.resize(topN);
    return v;

}

QVector<QPair<quint32,int>>GestorCalificaciones::usuariosQueMasCalifican(int topN) const
{


    QVector<RegistroCalificacion>all;
    leerTodas(all);
    QHash<quint32,int>cont; // userId -> count
    for(const auto&r:all)cont[r.userId]+=1;
    QVector<QPair<quint32,int>>v;v.reserve(cont.size());
    for(auto it=cont.begin();it!=cont.end();++it)v.push_back({it.key(),it.value()});
    std::sort(v.begin(), v.end(), [](auto a, auto b){ return a.second>b.second; });
    if (v.size()>topN) v.resize(topN);
    return v;

}

StatsUsuarioRate GestorCalificaciones::statsUsuario(quint32 userId, int ultimasN) const
{

    QVector<RegistroCalificacion>all;
    leerTodas(all);
    StatsUsuarioRate st;
    long long suma=0;
    for(const auto& r:all)if(r.userId==userId){

        suma+=r.rating; st.cantidad+=1; st.ultimas.push_back(r);
        if(st.ultimas.size()>ultimasN)st.ultimas.pop_front();

    }
    if(st.cantidad>0)st.promedioOtorgado=double(suma)/double(st.cantidad);
    return st;

}
