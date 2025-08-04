#include "tipos.h"

//Tipo
QString tipoToString(Tipo tipo)
{

    switch(tipo)
    {

    case Tipo::Single: return "Single";
    case Tipo::EP: return "EP";
    case Tipo::Album: return "Álbum";

    }
    return "";

}
Tipo stringToTipo(const QString& str)
{

    if(str=="EP") return Tipo::EP;
    if(str=="Álbum") return Tipo::Album;
    return Tipo::Single;

}

//Genero
QString generoToString(Genero g)
{

    switch (g)
    {

    case Genero::Pop: return "Pop";
    case Genero::Corridos: return "Corridos";
    case Genero::Cristianos: return "Cristianos";
    case Genero::Electronica: return "Electronica";
    case Genero::Regueton: return "Regueton";
    case Genero::Rock: return "Rock";
    case Genero::Clasicas: return "Clasicas";

    }
    return "";

}
Genero stringToGenero(const QString& str)
{

    if(str=="Corridos") return Genero::Corridos;
    if(str=="Cristianos") return Genero::Cristianos;
    if(str=="Electronica") return Genero::Electronica;
    if(str=="Regueton") return Genero::Regueton;
    if(str=="Rock") return Genero::Rock;
    if(str=="Clasicas") return Genero::Clasicas;
    return Genero::Pop;

}

//Categoria
QString categoriaToString(Categoria c)
{

    switch (c)
    {

    case Categoria::Instrumental: return "instrumental";
    case Categoria::Recomendado: return "recomendado";
    case Categoria::Favorito: return "favorito";
    case Categoria::Infantil: return "infantil";
    case Categoria::Playlist: return "playlist";

    }
    return "";

}
Categoria stringToCategoria(const QString& str)
{

    if(str=="recomendado") return Categoria::Recomendado;
    if(str=="favorito") return Categoria::Favorito;
    if(str=="infantil") return Categoria::Infantil;
    if(str=="instrumental") return Categoria::Instrumental;
    return Categoria::Playlist;

}
