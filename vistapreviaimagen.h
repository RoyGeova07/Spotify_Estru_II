#ifndef VISTAPREVIAIMAGEN_H
#define VISTAPREVIAIMAGEN_H

#include <QLabel>
#include <QEvent>
#include <QPropertyAnimation>

class VistaPreviaImagen : public QLabel
{
    Q_OBJECT

public:

    explicit VistaPreviaImagen(QWidget*parent=nullptr);

protected:

    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:

    int normalSize;
    int zoomSize;

};

#endif // VISTAPREVIAIMAGEN_H
