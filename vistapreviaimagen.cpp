#include "vistapreviaimagen.h"

VistaPreviaImagen::VistaPreviaImagen(QWidget*parent):QLabel(parent)
{

    normalSize=160;
    zoomSize=180;
    setFixedSize(normalSize,zoomSize);
    setStyleSheet("border: 1px solid #555; background-color: #111; color: white;");
    setAlignment(Qt::AlignCenter);
    setText("Sin Imagen");
    setCursor(Qt::PointingHandCursor);

}

void VistaPreviaImagen::enterEvent(QEnterEvent*event)
{

    setFixedSize(zoomSize,zoomSize);
    QLabel::enterEvent(event);

}

void VistaPreviaImagen::leaveEvent(QEvent*event)
{

    setFixedSize(normalSize,normalSize);
    QLabel::leaveEvent(event);

}
