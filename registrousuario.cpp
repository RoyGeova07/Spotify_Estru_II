#include"registrousuario.h"
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QFileDialog>
#include<QDir>
#include<QPixmap>
#include<QDate>
#include<QMessageBox>
#include"home.h"

RegistroUsuario::RegistroUsuario(QWidget *parent) : QDialog(parent)
{

    setWindowTitle("Registro de Usuario");
    setFixedSize(520, 500);

    txtNombreUsuario = new QLineEdit;
    txtContrasena = new QLineEdit;
    txtContrasena->setEchoMode(QLineEdit::Password);
    txtRutaImagen = new QLineEdit;
    txtRutaImagen->setVisible(false);

    comboPais = new QComboBox;
    comboPais->addItems(
    {
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

    comboGeneroMusical = new QComboBox;
    comboGeneroMusical->addItems({"Pop", "Rock", "Reguetón", "Electrónica", "Cristianos", "Clásicas"});

    vistaPreviaImagen = new QLabel("Sin imagen");
    vistaPreviaImagen->setFixedSize(120, 120);
    vistaPreviaImagen->setStyleSheet("border: 2px solid white; background-color: black; color: white;");
    vistaPreviaImagen->setAlignment(Qt::AlignCenter);

    btnSeleccionarImagen= new QPushButton("Seleccionar Imagen");
    connect(btnSeleccionarImagen, &QPushButton::clicked, this, &RegistroUsuario::SeleccionarImagen);

    btnRegistrar =new QPushButton("Registrar Usuario");
    btnRegistrar->setFixedWidth(180);
    btnRegistrar->setStyleSheet(R"(
        QPushButton {
            background-color: #1DB954;
            color: white;
            padding: 8px;
            font-weight: bold;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #1ed760;
        }
    )");
    connect(btnRegistrar, &QPushButton::clicked, this, &RegistroUsuario::RegistrarUsuario);

    btnVolver = new QPushButton("← Volver al Menú");
    btnVolver->setStyleSheet(R"(
        QPushButton {
            background-color: #444;
            color: white;
            padding: 6px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #666;
        }
    )");
    connect(btnVolver, &QPushButton::clicked, this, &RegistroUsuario::RegresarAlMenu);

    lblResultado = new QLabel;
    lblResultado->setStyleSheet("color: red");

    // Layout general
    QVBoxLayout *formLayout = new QVBoxLayout;
    formLayout->addWidget(new QLabel("Nombre de Usuario:")); formLayout->addWidget(txtNombreUsuario);
    formLayout->addWidget(new QLabel("Contraseña:")); formLayout->addWidget(txtContrasena);
    formLayout->addWidget(new QLabel("País de Origen:")); formLayout->addWidget(comboPais);
    formLayout->addWidget(new QLabel("Género Musical:")); formLayout->addWidget(comboGeneroMusical);

    // Imagen
    formLayout->addSpacing(10);
    QLabel* lblImagen = new QLabel("Imagen de Perfil:");
    lblImagen->setStyleSheet("font-weight: bold;");
    formLayout->addWidget(lblImagen);

    QVBoxLayout *imagenLayout = new QVBoxLayout;
    imagenLayout->setAlignment(Qt::AlignHCenter);
    imagenLayout->addWidget(vistaPreviaImagen);
    imagenLayout->addSpacing(15);
    imagenLayout->addWidget(btnSeleccionarImagen);
    formLayout->addLayout(imagenLayout);

    // Resultado centrado
    lblResultado->setAlignment(Qt::AlignHCenter);
    formLayout->addWidget(lblResultado);

    // Botones finales
    QHBoxLayout *botonesLayout =new QHBoxLayout;
    botonesLayout->setAlignment(Qt::AlignHCenter);
    botonesLayout->addWidget(btnVolver);
    botonesLayout->addSpacing(20);
    botonesLayout->addWidget(btnRegistrar);
    formLayout->addLayout(botonesLayout);

    setLayout(formLayout);
}

void RegistroUsuario::SeleccionarImagen()
{
    QString ruta =QFileDialog::getOpenFileName(this, "Seleccionar Imagen", QDir::homePath(), "Imágenes (*.png *.jpg *.jpeg)");
    if(!ruta.isEmpty())
    {

        txtRutaImagen->setText(ruta);
        QPixmap imagen(ruta);
        vistaPreviaImagen->setPixmap(imagen.scaled(vistaPreviaImagen->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    }
}

void RegistroUsuario::RegistrarUsuario()
{
    QString nombre = txtNombreUsuario->text();
    QString contrasena = txtContrasena->text();
    QString pais = comboPais->currentText();
    QString genero = comboGeneroMusical->currentText();
    QString rutaImagen = txtRutaImagen->text();

    if(nombre.isEmpty()||contrasena.isEmpty()||pais.isEmpty()||genero.isEmpty()||rutaImagen.isEmpty())
    {
        QMessageBox::warning(this, "Campos incompletos", "Por favor completa todos los campos obligatorios.");
        return;
    }

    QDate hoy=QDate::currentDate();

    if(!gestorUsuarios.registrarUsuario(nombre, contrasena, genero, rutaImagen, hoy, false))
    {

        QMessageBox::warning(this, "Registro fallido", "El nombre de usuario ya esta en uso.");
        return;

    }

    QMessageBox::information(this, "Registro exitoso", QString("El usuario '%1' ha sido registrado correctamente.").arg(nombre));

    lblResultado->setStyleSheet("color: green");
    lblResultado->setText("Usuario registrado correctamente.");

    Home*h=new Home(this);
    this->hide();
    h->exec();
    this->close();

}

void RegistroUsuario::RegresarAlMenu()
{

    this->close();

}
