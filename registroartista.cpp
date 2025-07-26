#include "registroartista.h"
#include <QMessageBox>
#include <QPixmap>
#include <QSpacerItem>
#include <QFile>
#include <QDebug>
#include <QComboBox>
#include <QFileDialog>
#include <QDir>
#include"home.h"

RegistroArtista::RegistroArtista(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Registro de Artista");
    setFixedSize(700, 740);

    // Campos
    txtContrasena = new QLineEdit;
    txtContrasena->setEchoMode(QLineEdit::Password);
    txtNombreArtistico = new QLineEdit;
    txtNombreReal = new QLineEdit;
    comboPais = new QComboBox;
    comboPais->addItems({
        "Afganistán", "Albania", "Alemania", "Andorra", "Angola", "Antigua y Barbuda", "Arabia Saudita", "Argelia", "Argentina",
        "Armenia", "Australia", "Austria", "Azerbaiyán", "Bahamas", "Bangladés", "Baréin", "Barbados", "Bélgica",
        "Belice", "Benín", "Bielorrusia", "Birmania", "Bolivia", "Bosnia y Herzegovina", "Botsuana", "Brasil",
        "Brunéi", "Bulgaria", "Burkina Faso", "Burundi", "Bután", "Cabo Verde", "Camboya", "Camerún", "Canadá",
        "Catar", "Chad", "Chile", "China", "Chipre", "Ciudad del Vaticano", "Colombia", "Comoras", "Corea del Norte",
        "Corea del Sur", "Costa de Marfil", "Costa Rica", "Croacia", "Cuba", "Dinamarca", "Dominica", "Ecuador",
        "Egipto", "El Salvador", "Emiratos Árabes Unidos", "Eritrea", "Eslovaquia", "Eslovenia", "España", "Estados Unidos",
        "Estonia", "Etiopía", "Filipinas", "Finlandia", "Fiyi", "Francia", "Gabón", "Gambia", "Georgia", "Ghana",
        "Granada", "Grecia", "Guatemala", "Guyana", "Guinea", "Guinea-Bisáu", "Guinea Ecuatorial", "Haití", "Honduras",
        "Hungría", "India", "Indonesia", "Irak", "Irán", "Irlanda", "Islandia", "Islas Marshall", "Islas Salomón",
        "Israel", "Italia", "Jamaica", "Japón", "Jordania", "Kazajistán", "Kenia", "Kirguistán", "Kiribati", "Kuwait",
        "Laos", "Lesoto", "Letonia", "Líbano", "Liberia", "Libia", "Liechtenstein", "Lituania", "Luxemburgo", "Macedonia del Norte",
        "Madagascar", "Malasia", "Malaui", "Maldivas", "Malí", "Malta", "Marruecos", "Mauricio", "Mauritania", "México",
        "Micronesia", "Moldavia", "Mónaco", "Mongolia", "Montenegro", "Mozambique", "Namibia", "Nauru", "Nepal",
        "Nicaragua", "Níger", "Nigeria", "Noruega", "Nueva Zelanda", "Omán", "Países Bajos", "Pakistán", "Palaos",
        "Panamá", "Papúa Nueva Guinea", "Paraguay", "Perú", "Polonia", "Portugal", "Reino Unido", "República Centroafricana",
        "República Checa", "República del Congo", "República Democrática del Congo", "República Dominicana", "Ruanda",
        "Rumanía", "Rusia", "Samoa", "San Cristóbal y Nieves", "San Marino", "San Vicente y las Granadinas", "Santa Lucía",
        "Santo Tomé y Príncipe", "Senegal", "Serbia", "Seychelles", "Sierra Leona", "Singapur", "Siria", "Somalia",
        "Sri Lanka", "Suazilandia", "Sudáfrica", "Sudán", "Sudán del Sur", "Suecia", "Suiza", "Surinam", "Tailandia",
        "Tanzania", "Tayikistán", "Timor Oriental", "Togo", "Tonga", "Trinidad y Tobago", "Túnez", "Turkmenistán", "Turquía",
        "Tuvalu", "Ucrania", "Uganda", "Uruguay", "Uzbekistán", "Vanuatu", "Venezuela", "Vietnam", "Yemen", "Yibuti",
        "Zambia", "Zimbabue"
    });
    txtBiografia = new QTextEdit;
    txtBiografia->setFixedHeight(80);
    txtRutaImagen = new QLineEdit;
    txtRutaImagen->setVisible(false);

    comboGeneroMusical = new QComboBox;
    comboGeneroMusical->addItems({"Pop", "Corridos", "Cristianos", "Electrónica", "Reguetón", "Rock", "Clásicas"});

    QString estiloInput = R"(
    QLineEdit, QComboBox {
        background-color: rgba(255,255,255,0.9);
        border: 1px solid gray;
        border-radius: 4px;
        padding: 5px;
        font-size: 14px;
        color: black;
    }
    QTextEdit {
        background-color: white;
        border: 1px solid gray;
        border-radius: 4px;
        padding: 5px;
        font-size: 14px;
        color: black;
    }
    QLabel {
        font-size: 14px;
        font-weight: bold;
        color: white;
    }
    )";

    txtContrasena->setStyleSheet(estiloInput);
    txtNombreArtistico->setStyleSheet(estiloInput);
    txtNombreReal->setStyleSheet(estiloInput);
    comboPais->setStyleSheet(estiloInput);
    comboGeneroMusical->setStyleSheet(estiloInput);
    txtBiografia->setStyleSheet(estiloInput);

    // Botón imagen
    btnSeleccionarImagen = new QPushButton("Seleccionar Imagen...");
    btnSeleccionarImagen->setStyleSheet(R"(
        QPushButton {
            background-color: #333;
            color: white;
            padding: 8px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #555;
        }
    )");

    // Vista previa
    vistaPreviaImagen = new QLabel("Sin imagen");
    vistaPreviaImagen->setFixedSize(140, 140);
    vistaPreviaImagen->setStyleSheet("border: 2px solid white; background-color: black; color: white;");
    vistaPreviaImagen->setAlignment(Qt::AlignCenter);

    // Botón registrar
    btnRegistrar = new QPushButton("Registrar Artista");
    btnRegistrar->setFixedWidth(220);
    btnRegistrar->setStyleSheet(R"(
        QPushButton {
            background-color: #1DB954;
            color: white;
            padding: 10px;
            font-size: 15px;
            font-weight: bold;
            border-radius: 8px;
        }
        QPushButton:hover {
            background-color: #1ed760;
        }
    )");

    // Botón volver
    btnVolver = new QPushButton("← Volver al Menú");
    btnVolver->setStyleSheet(R"(
        QPushButton {
            background-color: #444;
            color: white;
            padding: 8px;
            font-size: 14px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #666;
        }
    )");

    // Etiqueta resultado
    lblResultado = new QLabel;
    lblResultado->setStyleSheet("color: red; font-size: 13px;");

    // Layout formulario principal
    QVBoxLayout *formLayout = new QVBoxLayout;
    formLayout->addWidget(new QLabel("Nombre Artístico:")); formLayout->addWidget(txtNombreArtistico);
    formLayout->addWidget(new QLabel("Nombre Real:")); formLayout->addWidget(txtNombreReal);
    formLayout->addWidget(new QLabel("Contraseña:")); formLayout->addWidget(txtContrasena);
    formLayout->addWidget(new QLabel("País:"));formLayout->addWidget(comboPais);
    formLayout->addWidget(new QLabel("Género Musical:")); formLayout->addWidget(comboGeneroMusical);
    formLayout->addWidget(new QLabel("Biografía:")); formLayout->addWidget(txtBiografia);

    // Etiqueta bien presentada
    formLayout->addSpacing(10);
    QLabel* lblImagen = new QLabel("Imagen del Artista:");
    lblImagen->setStyleSheet("font-size: 14px; font-weight: bold; color: white;");
    formLayout->addWidget(lblImagen);

    // Layout vertical para que imagen quede debajo
    QVBoxLayout *imagenLayout = new QVBoxLayout;
    imagenLayout->setAlignment(Qt::AlignHCenter);
    imagenLayout->addWidget(vistaPreviaImagen);
    imagenLayout->addSpacing(1);
    imagenLayout->addWidget(btnSeleccionarImagen);
    imagenLayout->addSpacing(1);
    formLayout->addLayout(imagenLayout);

    // Resultado centrado
    lblResultado->setAlignment(Qt::AlignHCenter);
    formLayout->addWidget(lblResultado);

    // Layout horizontal para botones inferiores
    QHBoxLayout* botonesLayout = new QHBoxLayout;
    botonesLayout->setAlignment(Qt::AlignHCenter);
    botonesLayout->addWidget(btnVolver);
    botonesLayout->addSpacing(20);
    botonesLayout->addWidget(btnRegistrar);
    formLayout->addLayout(botonesLayout);
    formLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Conexiones
    connect(btnRegistrar, &QPushButton::clicked, this, &RegistroArtista::RegistrarArtista);
    connect(btnSeleccionarImagen, &QPushButton::clicked, this, [=]() {
        QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar Imagen", QDir::homePath(), "Imágenes (*.png *.jpg *.jpeg)");
        if (!ruta.isEmpty()) {
            txtRutaImagen->setText(ruta);
            QPixmap imagen(ruta);
            vistaPreviaImagen->setText("");
            vistaPreviaImagen->setPixmap(imagen.scaled(vistaPreviaImagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    });
    connect(btnVolver, &QPushButton::clicked, this, &RegistroArtista::RegresarAlMenu);

}

void RegistroArtista::RegistrarArtista()
{
    QString contrasena =txtContrasena->text();
    QString nombreArtistico= txtNombreArtistico->text();
    QString nombreReal =txtNombreReal->text();
    QString pais =comboPais->currentText();
    QString genero= comboGeneroMusical->currentText();
    QString biografia= txtBiografia->toPlainText();
    QString rutaImagen= txtRutaImagen->text();

    if(contrasena.isEmpty()||nombreArtistico.isEmpty()||nombreReal.isEmpty()||pais.isEmpty()||genero.isEmpty()||biografia.isEmpty()||rutaImagen.isEmpty())
    {

        QMessageBox::warning(this,"Campos incompletos","Por favor completa todos los campos obligatorios.");
        return;

    }

    QDate hoy=QDate::currentDate();

    int idGenerado=1;
    QVector<Artista> artistas=gestorArtistas.leerArtista();
    for(const Artista &a:artistas)
    {

        if(a.getId()>=idGenerado)
            idGenerado=a.getId()+1;

    }

    Artista artista(idGenerado, nombreArtistico, nombreReal, pais, genero, biografia, contrasena, rutaImagen);
    if(!gestorArtistas.GuardarArtista(artista))
    {

        QMessageBox::warning(this,"Registro fallido","El nombre artístico ya esta en uso.");
        return;

    }

    QMessageBox::information(this, "Registro exitoso", QString("El artista '%1' ha sido registrado correctamente.").arg(nombreArtistico));

    lblResultado->setStyleSheet("color: green;");
    lblResultado->setText("Artista registrado correctamente.");

    Home* home=new Home(this);
    this->hide();
    home->exec();
    this->close();

}

void RegistroArtista::RegresarAlMenu() {
    this->close();
}

