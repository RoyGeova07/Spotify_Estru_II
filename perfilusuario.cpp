#include "perfilusuario.h"
#include <QPixmap>
#include"home.h"

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
    gridCards->setHorizontalSpacing(16);
    gridCards->setVerticalSpacing(16);

    gridCards->addWidget(crearTarjeta("Canciones escuchadas", "0", "Total histórico"), 0,0);
    gridCards->addWidget(crearTarjeta("Tiempo de reproducción", "00:00:00", "hh:mm:ss"), 0,1);
    gridCards->addWidget(crearTarjeta("Promedio de calificaciones", "0.0", "De 1 a 5"), 0,2);

    lay->addLayout(gridCards);

    // ---- Canciones mas escuchadas (tabla demo) ----
    {

        auto lbl = new QLabel("Tus canciones más escuchadas");
        lbl->setStyleSheet("font-size:18px; font-weight:600;");
        lay->addWidget(lbl);

        auto tablaTop = crearTabla({"Título", "Artista", "Reproducciones"}, 5);
        lay->addWidget(tablaTop);

    }

    // ---- Ultimas canciones calificadas (tabla demo) ----
    {

        auto lbl=new QLabel("Ultimas canciones calificadas");
        lbl->setStyleSheet("font-size:18px; font-weight:600;");
        lay->addWidget(lbl);

        auto tablaCal=crearTabla({"Título", "Artista", "Calificación", "Fecha"}, 5);
        lay->addWidget(tablaCal);

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
