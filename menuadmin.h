#ifndef MENUADMIN_H
#define MENUADMIN_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include "artista.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QVector>
#include "vistapreviaimagen.h"
#include <QListWidget>
#include "controlreproduccion.h"
#include "sliderclickable.h"
#include <QPointer>   // Evita punteros colgantes con deleteLater().
#include <QHeaderView>

class MenuAdmin : public QWidget
{
    Q_OBJECT

public:
    explicit MenuAdmin(const Artista& artistaActivo, QWidget* parent=nullptr);

private slots:
    void abrirVentanaSubirCancion();
    void CerrarSesion();

    void seleccionarAudio();//por pista (sigue vigente)
    void tipoSeleccionado(const QString& tipo);
    void subirCanciones();

    // Mantengo este nombre por compatibilidad, pero ahora usa la imagen global:
    void SeleccionarImagen();
    void seleccionarImagenGlobal();

    void mostrarDatosCancionActual(int index);
    void MostrarPerfil();
    void MostrarEstadisticas();
    void seleccionarAudiosMultiples();

private:
    Artista artista;
    QLabel*lblFoto{};
    QLabel*lblNombre{};
    QPushButton*btnSubirCancion{};
    QPushButton*btnEditarCancion{};
    QPushButton*btnEliminarCancion{};
    QPushButton*btnVerEstadisticas{};
    QPushButton*btnSalir{};
    QPushButton*btnMiMusica{};
    QPushButton*btnPerfil{};

    void configurarUI();
    void estiloBoton(QPushButton* boton, const QString& color);

    //=========================================================
    QLabel*lblNombreArtista{};
    QComboBox*comboTipo{};           // Single, EP, Álbum
    QTabWidget*tabsCanciones{};
    QPushButton*btnSubir{};

    struct WidgetCancion
    {

        QLabel*lblIdCancion{};
        QLineEdit*txtTitulo{};
        QLineEdit*txtDuracion{};
        QLineEdit*txtRutaAudio{};

    };

    void mostrarReproductorAdmin(const QVector<Cancion>& canciones);
    void MostrarPanelMiMusica();

    QVector<WidgetCancion>cancionesWidgets;
    void LimpiarPanelDerecho();

    void crearPestañasCanciones(int cantidad);
    QWidget* crearFormularioCancion(int index,int ID);

    QVBoxLayout* layoutDerecho{};

    QPointer<QWidget> panelReproductorAdmin=nullptr;
    QPointer<ControlReproduccion>controlAdmin=nullptr;

    QPointer<QLabel>lblCaratula=nullptr;
    QPointer<QLabel>lblTitulo=nullptr;
    QPointer<QLabel>lblArtista= nullptr;
    QPointer<QLabel>lblTipo =nullptr;
    QPointer<QLabel>lblReproducciones=nullptr;
    QPointer<QLabel>lblDescripcion=nullptr;
    QPointer<QLabel>lblGenero=nullptr;
    QPointer<QLabel>lblFechaCarga=nullptr;

    QPointer<SliderClickable> barraProgreso=nullptr;
    QPointer<QLabel> lblTiempoActual=nullptr;
    QPointer<QLabel> lblDuracionTotal=nullptr;

    QPointer<QPushButton>btnAnterior=nullptr;
    QPointer<QPushButton>btnPlayPause=nullptr;
    QPointer<QPushButton>btnSiguiente=nullptr;
    QPointer<QPushButton>btnRepetir=nullptr;
    QPointer<QPushButton>btnAleatorio=nullptr;
    QPointer<QPushButton>btnEditarCancionReproductor=nullptr;
    QPointer<QPushButton>btnEliminarCancionReproductor=nullptr;

    // ====== Campos GLOBALES de la publicación (EP / Album) ======
    QLineEdit*txtNombrePublicacion=nullptr;
    QComboBox*comboGeneroGlobal=nullptr;
    QComboBox*comboCategoriaGlobal=nullptr;
    QLineEdit*txtRutaImagenGlobal=nullptr;
    VistaPreviaImagen*vistaPortadaGlobal=nullptr;
    QPlainTextEdit* txtDescripcionGlobal=nullptr;

    QPushButton* btnSeleccionarAudios=nullptr;

    QPointer<QListWidget>listaWidget=nullptr;

    QVector<Cancion>listaCanciones;

    QString formatearTiempo(qint64 milisegundos);
    void actualizarTiempo(qint64 posicion);
    void actualizarDuracion(qint64 duracion);
    void ApagarReproductor();
    void refrescarSidebarFotoNombre();

    QWidget* crearTarjetaSeccion(const QString& titulo, QWidget* contenido);
    QWidget* tablaDummy(const QStringList& cabeceras,int filas,bool conRanking=false);
};

#endif // MENUADMIN_H

