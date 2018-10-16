#include "circlebutton.h"

CircleButton::CircleButton(QWidget *parent) : QLabel(parent)
{
    generateIcon();

    this->setPixmap(*this->buttonIcon);
    this->setLineWidth(3);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
}

void CircleButton::generateIcon() {
    QPixmap *pix = new QPixmap(50, 50);
    pix->fill(Qt::transparent);
    QPainter painter(pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);
    painter.drawEllipse(1, 1, 48, 48);
    painter.end();

    this->buttonIcon = pix;
}

void CircleButton::mousePressEvent(QMouseEvent *event)
{
    if (this->clicked) {
        this->buttonOff();
    } else {
        this->buttonOn();
    }

    QLabel::mousePressEvent(event);
}

void CircleButton::buttonOn() {
    this->setFrameShadow(QFrame::Sunken);
    this->clicked = true;
    emit circleOn();
}

void CircleButton::buttonOff() {
    this->setFrameShadow(QFrame::Raised);
    this->clicked = false;
    emit circleOff();
}
