QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    artista.cpp \
    cancion.cpp \
    gestorartistas.cpp \
    gestorcanciones.cpp \
    gestorusuarios.cpp \
    home.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    menuadmin.cpp \
    menuinicio.cpp \
    registroartista.cpp \
    registrousuario.cpp \
    subir_cancion.cpp \
    usuario.cpp \
    vistapreviaimagen.cpp

HEADERS += \
    artista.h \
    cancion.h \
    gestorartistas.h \
    gestorcanciones.h \
    gestorusuarios.h \
    home.h \
    login.h \
    mainwindow.h \
    menuadmin.h \
    menuinicio.h \
    registroartista.h \
    registrousuario.h \
    subir_cancion.h \
    usuario.h \
    vistapreviaimagen.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    Proyecto1_Estru2_Spotify_es_419.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets.qrc
