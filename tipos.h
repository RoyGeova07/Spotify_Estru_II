#ifndef TIPOS_H
#define TIPOS_H

#include<QString>

//Enum para Tipo
enum class Tipo{Single,EP,Album};
QString tipoToString(Tipo tipo);
Tipo stringToTipo(const QString& str);

//Enum para Genero
enum class Genero{Pop,Corridos,Cristianos,Electronica,Regueton,Rock,Clasicas};
QString generoToString(Genero g);
Genero stringToGenero(const QString& str);

//Enum para Categoria
enum class Categoria{Instrumental,Recomendado,Favorito,Infantil,Playlist};
QString categoriaToString(Categoria c);
Categoria stringToCategoria(const QString& str);

#endif // TIPOS_H
