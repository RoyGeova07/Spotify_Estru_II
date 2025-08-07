#ifndef SLIDERCLICKABLE_H
#define SLIDERCLICKABLE_H

#include <QSlider>
#include <QMouseEvent>
#include<QStyle>

//ESTO ME SIRVE PARA QUE EL USUARIO LE DE CLICK EN CUALQUIER PARTE DEL SLIDER Y QUE SE ACTUALIZE LA POSICION DE LA CANCION
//DE INMEDIATO
class SliderClickable:public QSlider

{
public:

    using QSlider::QSlider;//PARA HEREDAR CONSTRUCTORES

protected:

    void mousePressEvent(QMouseEvent*event)override
    {

        if(event->button()==Qt::LeftButton)
        {

            int val=QStyle::sliderValueFromPosition(minimum(),maximum(),event->position().x(),width());
            setValue(val);
            emit sliderMoved(val);
            emit sliderReleased();//PARA QUE SE ACTUALIZE EL REPRODUCTOR

        }
        QSlider::mousePressEvent(event);

    }

    SliderClickable();
};

#endif // SLIDERCLICKABLE_H
