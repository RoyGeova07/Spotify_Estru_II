#include "perfilusuario.h"
#include <QPixmap>
#include"home.h"
#include"gestorreproduccion.h"
#include"gestorcanciones.h"
#include"cancion.h"
#include"gestorcalificaciones.h"
#include<QDateTime>
#include<QStyledItemDelegate>
#include<QDir>

namespace
{

class CenterDelegate:public QStyledItemDelegate
{

public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void initStyleOption(QStyleOptionViewItem* opt, const QModelIndex& idx) const override
    {

        QStyledItemDelegate::initStyleOption(opt, idx);
        opt->displayAlignment =Qt::AlignCenter; // centro horizontal y vertical

    }
};

} // namespace

namespace {

// Busca (por songId) en todos los .dat del usuario y devuelve título/artista.
// Devuelve true si encontró datos.
static bool metaDesdePlaylists(const Usuario& u, quint32 songId,QString* outTitulo, QString* outArtista)
{
    const QString baseDir ="Publico/Usuario_"+u.getNombreUsuario();
    QDir dir(baseDir);
    if (!dir.exists()) return false;

    const QStringList archivos=dir.entryList(QStringList() << "*.dat", QDir::Files, QDir::Name);
    for(const QString& f : archivos)
    {

        QFile in(dir.filePath(f));
        if (!in.open(QIODevice::ReadOnly)) continue;

        QDataStream ds(&in);
        ds.setVersion(QDataStream::Qt_5_15);
        while(!ds.atEnd())
        {
            quint32 magic =0; quint16 ver=0;
            ds>>magic>>ver;
            if(ds.status()!=QDataStream::Ok)break;
            if(magic!=0x534F4E47) break; // 'SONG' inválido

            quint32 id=0;
            QString titulo,artista, descripcion, rutaAudio, rutaImagen, duracionStr;
            quint16 tipo=0,genero=0;
            QDate  fecha;
            qint32  reproducciones =0;
            bool activa =false;

            // Formato v2
            if(ver>=2)
            {

                ds>>id;

            }
            ds>>titulo;
            ds>>artista;
            ds>>tipo;
            ds>>descripcion;
            ds>>genero;
            ds>>fecha;
            ds>>duracionStr;
            ds>>rutaAudio;
            ds>>rutaImagen;
            ds>>reproducciones;
            ds>>activa;

            if(ver>=2&&id==songId)
            {

                if(outTitulo)*outTitulo= titulo;
                if(outArtista)*outArtista =artista;
                return true;

            }
        }
    }
    return false;
}

} // namespace


PerfilUsuario::PerfilUsuario(const Usuario&u,QWidget*parent):QWidget(parent),usuario(u)
{

    setWindowTitle("Perfil de Usuario");
    setFixedSize(900, 700);
    setStyleSheet("background-color:#121212; color:white;");

    auto root=new QVBoxLayout(this);
    root->setContentsMargins(24,24,24,24);
    root->setSpacing(18);

    auto header=new QLabel("Mi Perfil");
    header->setStyleSheet("font-size:22px; font-weight:700;");
    root->addWidget(header);

    //Superior: foto + nombre/grilla
    auto top=new QHBoxLayout;
    top->setSpacing(20);

    foto=new QLabel;
    foto->setFixedSize(140,140);
    foto->setStyleSheet("border-radius:70px; border:3px solid #1DB954; background:#222;");
    top->addWidget(foto,0,Qt::AlignTop);
    cargarFotoCircular(usuario.getRutaFoto(),foto->size());

    auto right=new QVBoxLayout;

    lblNombreUsuario=new QLabel(usuario.getNombreUsuario());
    lblNombreUsuario->setStyleSheet("font-size:26px; font-weight:800;");
    right->addWidget(lblNombreUsuario);


    auto grid=new QGridLayout; grid->setHorizontalSpacing(16);grid->setVerticalSpacing(8);

    auto mk=[](const QString& k, const QString& v)
    {

        QWidget*w=new QWidget;auto h=new QHBoxLayout(w);h->setContentsMargins(0,0,0,0);
        auto lk=new QLabel(k + ":");lk->setStyleSheet("color:#b3b3b3; font-size:14px;");

        auto lv=new QLabel(v);
        lv->setObjectName("valor");
        lv->setStyleSheet("color:white;   font-size:14px;");

        h->addWidget(lk); h->addWidget(lv,1);return w;

    };

    lblId=new QLabel(QString::number(usuario.getId()));
    lblNombreReal=new QLabel(usuario.getNombreReal());
    lblCorreo=new QLabel(usuario.getCorreo());
    lblGeneroFav=new QLabel(usuario.getGeneroFavorito());
    lblFechaReg=new QLabel(usuario.getFechaRegistro().toString("yyyy-MM-dd"));
    lblFechaNac=new QLabel(usuario.getFechaNacimiento().toString("yyyy-MM-dd"));
    lblRol=new QLabel(usuario.getEsAdmin()?"Administrador":"Usuario");
    lblEstado=new QLabel(usuario.estaActivo()?"Activo":"Inactivo");

    grid->addWidget(mk("ID",lblId->text()),0,0);
    grid->addWidget(mk("Correo",lblCorreo->text()),0,1);
    grid->addWidget(mk("Nombre real",lblNombreReal->text()),1,0);
    grid->addWidget(mk("Género favorito",lblGeneroFav->text()),1,1);
    grid->addWidget(mk("Registro",lblFechaReg->text()),2,0);
    grid->addWidget(mk("Nacimiento",lblFechaNac->text()),2,1);
    grid->addWidget(mk("Rol",lblRol->text()),3,0);
    grid->addWidget(mk("Estado",lblEstado->text()),3,1);

    right->addLayout(grid);
    top->addLayout(right,1);

    root->addLayout(top);


    // =================================================================
    //                       MIS ESTADISTICAS (FRONT)
    // =================================================================
    auto tituloStats = new QLabel("Mis estadísticas");
    tituloStats->setStyleSheet("font-size:22px; font-weight:700; margin-top:10px;");
    root->addWidget(tituloStats);

    //AREA CON SCROLL VERTICAL
    auto scroll=new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea{border:none;}");

    auto cont=new QWidget;
    auto lay=new QVBoxLayout(cont);
    lay->setContentsMargins(0,6,0,6);
    lay->setSpacing(16);

    // ---- Tarjetas de resumen (placeholders) ----
    auto gridCards=new QGridLayout;
    gridCards->setSpacing(16);

    cardEscuchas=crearTarjeta("Canciones escuchadas", "0", "Cuenta ≥ 30s");
    cardTiempo=crearTarjeta("Tiempo de reproducción", "0:00:00", "Incluye parciales.");
    cardPromedio =crearTarjeta("Promedio de calificaciones", "0.0", "Promedio de tus calificaciones (1–5).");

    gridCards->addWidget(cardEscuchas,0,0);
    gridCards->addWidget(cardTiempo,0,1);
    gridCards->addWidget(cardPromedio,0,2);

    lay->addLayout(gridCards);
    // ---- Canciones mas escuchadas (tabla demo) ----
    {

        auto lbl = new QLabel("Tus canciones más escuchadas");
        lbl->setStyleSheet("font-size:18px; font-weight:600;");
        lay->addWidget(lbl);

        this->tablaTop = crearTabla({"Título", "Artista", "Reproducciones"}, 5);
        lay->addWidget(this->tablaTop);

        this->tablaTop->setItemDelegateForColumn(0,new CenterDelegate(this->tablaTop));
        this->tablaTop->setItemDelegateForColumn(3,new CenterDelegate(this->tablaTop));

        cargarEstadisticas();

    }

    // ---- Ultimas canciones calificadas (tabla demo) ----
    {
        auto lbl = new QLabel("Ultimas canciones calificadas");
        lbl->setStyleSheet("font-size:18px; font-weight:600;");
        lay->addWidget(lbl);

        auto tablaCal = crearTabla({"Título", "Artista", "Calificación", "Fecha"}, 5);
        lay->addWidget(tablaCal);

        tablaCal->setItemDelegateForColumn(0,new CenterDelegate(tablaCal)); //#
        tablaCal->setItemDelegateForColumn(1,new CenterDelegate(tablaCal)); //Titulo
        tablaCal->setItemDelegateForColumn(2,new CenterDelegate(tablaCal)); //Artista
        tablaCal->setItemDelegateForColumn(3,new CenterDelegate(tablaCal)); //Calificacion
        tablaCal->setItemDelegateForColumn(4,new CenterDelegate(tablaCal)); //Fecha

        //====== llenar la tabla con calificaciones del usuario ======
        GestorCalificaciones gcal;
        const auto su=gcal.statsUsuario(static_cast<quint32>(usuario.getId()),10);

        if(cardPromedio)
        {

            if(auto v=cardPromedio->findChild<QLabel*>("valor"))
            {

                v->setText(su.cantidad>0?QString::number(su.promedioOtorgado, 'f', 1):QStringLiteral("—"));

            }

        }

        // Mapa songId -> Cancion (solo catalogo vigente en canciones.dat)
        GestorCanciones gc;
        const auto todas=gc.leerCanciones();
        QHash<quint32,Cancion>porId;
        porId.reserve(todas.size());
        for(const auto&c:todas)
            porId.insert(static_cast<quint32>(c.getId()),c);

        // Tomar las 5 mas recientes CUYAS canciones sigan vigentes en canciones.dat
        QVector<RegistroCalificacion> ult;
        ult.reserve(5);
        for(int i=su.ultimas.size()-1;i>=0&&ult.size()<5;--i)
        {

            const auto& r=su.ultimas[i];
            if(porId.contains(r.songId))
            {

                ult.push_back(r);  //si la cancion fue borrada, NO se agrega (sin rastro)

            }

        }

        //La tabla ya viene con 5 filas “—”; solo rellenamos las N primeras validas
        const int N=ult.size();
        for(int i=0;i<N;++i)
        {

            const auto& r=ult[i];
            const auto& c= porId[r.songId];

            // Col 0: "#"
            auto posIt=new QTableWidgetItem(QString::number(i+1));
            posIt->setTextAlignment(Qt::AlignCenter);
            posIt->setFlags(posIt->flags() & ~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
            tablaCal->setItem(i,0,posIt);

            // Columnas reales (sin fallback a playlist ni “ID %1”)
            auto itTitulo=new QTableWidgetItem(c.getTitulo());
            auto itArtista= new QTableWidgetItem(c.getNombreArtista());
            auto itRate=new QTableWidgetItem(QString::number(r.rating));
            auto itFecha= new QTableWidgetItem(
                QDateTime::fromMSecsSinceEpoch(r.epochMs).toString("yyyy-MM-dd HH:mm")
                );

            itTitulo->setTextAlignment(Qt::AlignCenter);
            itArtista->setTextAlignment(Qt::AlignCenter);
            itRate->setTextAlignment(Qt::AlignCenter);
            itFecha->setTextAlignment(Qt::AlignCenter);

            itTitulo->setFlags(itTitulo->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
            itArtista->setFlags(itArtista->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
            itRate->setFlags(itRate->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
            itFecha->setFlags(itFecha->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);

            tablaCal->setItem(i,1,itTitulo);
            tablaCal->setItem(i,2,itArtista);
            tablaCal->setItem(i,3,itRate);
            tablaCal->setItem(i,4,itFecha);

        }
    }


    //OPCIONAL
    auto sep=new QFrame; sep->setFrameShape(QFrame::HLine); sep->setStyleSheet("color:#333;");
    lay->addWidget(sep);

    lay->addSpacing(8);

    scroll->setWidget(cont);
    root->addWidget(scroll,1);

    //Cerrar
    auto btnCerrar=new QPushButton("Cerrar");
    btnCerrar->setStyleSheet("QPushButton{background:#282828;color:white;border:1px solid #444;border-radius:8px;padding:6px 12px;} QPushButton:hover{background:#333;}");
    root->addStretch();
    root->addWidget(btnCerrar,0,Qt::AlignRight);

    connect(btnCerrar,&QPushButton::clicked,this,&PerfilUsuario::VolverHome);

}

QWidget* PerfilUsuario::crearTarjeta(const QString &titulo, const QString &valor, const QString &nota)
{

    auto card=new QWidget;
    card->setStyleSheet(
        "QWidget{background:#1a1a1a; border:1px solid #333; border-radius:12px;}"
        );
    auto v=new QVBoxLayout(card);
    v->setContentsMargins(14,12,14,12);
    v->setSpacing(6);

    auto lt=new QLabel(titulo);
    lt->setStyleSheet("color:#bdbdbd; font-size:13px;");

    auto lv=new QLabel(valor);
    lv->setObjectName("valor");
    lv->setStyleSheet("color:white; font-size:28px; font-weight:800;");

    v->addWidget(lt);
    v->addWidget(lv);

    if(!nota.isEmpty())
    {

        auto ln=new QLabel(nota);
        ln->setStyleSheet("color:#9e9e9e; font-size:12px;");
        v->addWidget(ln);

    }
    return card;

}
QTableWidget* PerfilUsuario::crearTabla(const QStringList &cabeceras, int filasDemo)
{
    // Insertamos una columna extra al inicio para la posicion
    QStringList cabecerasConPos = cabeceras;
    cabecerasConPos.insert(0, "#"); // Columna de posicion

    auto tabla = new QTableWidget(filasDemo, cabecerasConPos.size());

    // Cabeceras
    tabla->setHorizontalHeaderLabels(cabecerasConPos);
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Apariencia / UX
    tabla->verticalHeader()->setVisible(false);
    tabla->setShowGrid(false);
    tabla->setSelectionMode(QAbstractItemView::NoSelection);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tabla->setFocusPolicy(Qt::NoFocus);
    tabla->viewport()->setFocusPolicy(Qt::NoFocus);

    // Altura de filas y altura total de la tabla
    const int rowH = 36;
    tabla->verticalHeader()->setDefaultSectionSize(rowH);
    const int headerH = tabla->horizontalHeader()->height();
    const int minH = headerH + filasDemo * rowH + 24;
    tabla->setMinimumHeight(qMax(260, minH));

    // Estilos
    tabla->setStyleSheet(
        "QHeaderView::section{background:#222;color:#ddd;border:none;padding:8px;font-weight:600;}"
        "QTableWidget{background:#111;color:#ddd;border:1px solid #333;}"
        "QTableWidget::item{padding:8px;font-size:14px;}"
        );

    // Llenar filas demo con numeros de posicion y guiones
    for(int r=0; r< filasDemo; ++r)
    {
        // Columna de posicion (1, 2, 3, ...)
        auto* posItem=new QTableWidgetItem(QString::number(r+1));
        posItem->setTextAlignment(Qt::AlignCenter);
        tabla->setItem(r,0,posItem);

        // Resto de columnas con guiones
        for (int c = 1; c < cabecerasConPos.size(); ++c)
        {

            auto* it = new QTableWidgetItem("—");
            it->setTextAlignment(Qt::AlignVCenter | Qt::AlignCenter);
            tabla->setItem(r,c,it);

        }
    }

    return tabla;
}

void PerfilUsuario::cargarFotoCircular(const QString &ruta, const QSize &size)
{


    QPixmap px(ruta);
    if(px.isNull()){foto->setPixmap(QPixmap());return;}

    int lado=qMin(px.width(),px.height());
    QRect rec((px.width()-lado)/2,(px.height()-lado)/2,lado,lado);
    QPixmap cuadrado=px.copy(rec).scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    QPixmap circular(size);
    circular.fill(Qt::transparent);
    QPainter p(&circular);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path; path.addEllipse(0,0,size.width(),size.height());
    p.setClipPath(path);
    p.drawPixmap(0,0,cuadrado);
    p.end();

    foto->setPixmap(circular);

}

void PerfilUsuario::VolverHome()
{

    Home*h=new Home(usuario,nullptr);
    h->show();
    this->hide();

}

void PerfilUsuario::cargarEstadisticas()
{

    // 1)Leer estadisticas del usuario
    GestorReproduccion gr("reproducciones.dat");
    auto st=gr.statsUsuario(static_cast<quint32>(usuario.getId()),5);//solo mostrar el top 5

    // 2) Volcar a tarjetas
    if(cardEscuchas)
    {

        if(auto lbl = cardEscuchas->findChild<QLabel*>("valor"))
            lbl->setText(QString::number(st.totalEscuchas));

    }
    if(cardTiempo)
    {

        if(auto lbl=cardTiempo->findChild<QLabel*>("valor"))
            lbl->setText(aHMS(static_cast<quint64>(st.totalMs)));

    }

    // Promedio de calificaciones otorgadas (1–5) del usuario
    {
        GestorCalificaciones gcal;
        const auto suRates=gcal.statsUsuario(static_cast<quint32>(usuario.getId()),10);
        if(cardPromedio)
        {

            if(auto v=cardPromedio->findChild<QLabel*>("valor"))
            {

                v->setText(suRates.cantidad>0?QString::number(suRates.promedioOtorgado,'f',1):QStringLiteral("—"));

            }

        }
    }

    // 3) Tabla “Tus canciones mas escuchadas”
    if(!tablaTop) return;

    //Tabla fija de 5 filas. Tambien la dejamos solo-lectura y sin foco/seleccion.
    tablaTop->clearContents();
    tablaTop->setRowCount(5);
    tablaTop->setSelectionMode(QAbstractItemView::NoSelection);
    tablaTop->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaTop->setFocusPolicy(Qt::NoFocus);
    tablaTop->viewport()->setFocusPolicy(Qt::NoFocus);

    // Mapa songId -> Cancion (para mostrar títulos/artistas)
    GestorCanciones gc;
    const auto todas=gc.leerCanciones();
    QHash<quint32,Cancion> porId;
    porId.reserve(todas.size());
    for(const auto&c:todas)porId.insert(static_cast<quint32>(c.getId()), c);


    const int N=qMin(5,st.top.size());//N<5
    for(int i=0; i<N; ++i)
    {

        const auto songId=st.top[i].first;
        const int veces=st.top[i].second;
        if(!porId.contains(songId))continue;//no mostrar nada si u

        QString titulo=QString("ID %1").arg(songId);
        QString artista=QStringLiteral("—");
        if(porId.contains(songId))
        {

            const auto&c=porId[songId];
            titulo=c.getTitulo();
            artista=c.getNombreArtista();

        }

        //Columna # (1..5)
        auto pos=new QTableWidgetItem(QString::number(i+1));
        pos->setTextAlignment(Qt::AlignCenter);
        pos->setFlags(pos->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
        tablaTop->setItem(i,0,pos);

        // Título
        auto tIt=new QTableWidgetItem(titulo);
        tIt->setFlags(tIt->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
        tablaTop->setItem(i,1,tIt);

        // Artista
        auto aIt=new QTableWidgetItem(artista);
        aIt->setFlags(aIt->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
        tablaTop->setItem(i,2,aIt);

        // Reproducciones
        auto nIt=new QTableWidgetItem(QString::number(veces));
        nIt->setTextAlignment(Qt::AlignCenter);
        nIt->setFlags(nIt->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
        tablaTop->setItem(i,3,nIt);

    }

    // Completar filas vacias (si N < 5) con numeración y “—”
    for(int r=N; r<5;++r)
    {

        auto pos=new QTableWidgetItem(QString::number(r+1));
        pos->setTextAlignment(Qt::AlignCenter);
        pos->setFlags(pos->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
        tablaTop->setItem(r,0,pos);

        for(int c=1;c<=3;++c)
        {

            auto dash=new QTableWidgetItem(QStringLiteral("—"));
            dash->setTextAlignment(Qt::AlignCenter);
            dash->setFlags(dash->flags()&~Qt::ItemIsEditable&~Qt::ItemIsSelectable);
            tablaTop->setItem(r,c,dash);

        }
    }

}
