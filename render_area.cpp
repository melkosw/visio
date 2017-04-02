
#include "render_area.h"
#include <iostream>
#include <QImage>
#include <QPainter>

RatioRenderArea::RatioRenderArea(double r, QWidget *parent): RenderArea(parent),ratio(r) {

    QSizePolicy pol = this->sizePolicy();
    pol.setHeightForWidth(true);
    this->setSizePolicy(pol);

}

RenderArea::RenderArea(QWidget *parent): QWidget(parent)
{
    antialiased = false;
    sheet = new QImage(2048,2048,QImage::Format_RGB888);
    x=-1;
    y=-1;
    leftButton=false;
    rightButton=false;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);

}

RenderArea::RenderArea(int x, int y, QWidget *parent): QWidget(parent)
{
    antialiased = false;
    sheet = new QImage(x,y,QImage::Format_RGB888);
    x=-1;
    y=-1;
    leftButton=false;
    rightButton=false;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);

}

void RenderArea::setDim(int x,int y) {

    if (sheet) delete sheet;
    sheet = new QImage(x,y,QImage::Format_RGB888);

}

RenderArea::~RenderArea()
{
    delete sheet;
}

void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}


void RenderArea::paintEvent(QPaintEvent *) {

    QPainter painter(this);
    //QRect  dirtyrect = event->rect();
    painter.drawImage(QPoint(0,0),*sheet);

}


void RenderArea::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        leftButton=true;
        x=event->x();
        y=event->y();
        emit leftButtonPressed();
    }

    if (event->button() == Qt::RightButton) {
        rightButton=true;
        x=event->x();
        y=event->y();
        emit rightButtonPressed();
    }


}

void RenderArea::mouseReleaseEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
        leftButton=false;
        x=event->x();
        y=event->y();
        emit leftButtonReleased();
    }

    if (event->button() == Qt::RightButton) {
        rightButton=false;
        x=event->x();
        y=event->y();
        emit rightButtonReleased();
    }

}

void RenderArea::mouseMoveEvent(QMouseEvent *event) {

    emit mouseMove();
    x=event->x();
    y=event->y();
    if (event->buttons()==Qt::LeftButton) {
        //leftButton=false;
        emit leftDrag();
    }

    if (event->buttons()==Qt::RightButton) {
        //rightButton=false;
        emit rightDrag();
    }

}

void RenderArea::wheelEvent(QWheelEvent *event) {

    emit wheelMouse(event->delta());

}
