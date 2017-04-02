#ifndef RENDER_AREA_H
#define RENDER_AREA_H

#include <QWidget>
#include <QImage>
#include <QMouseEvent>
#include <QWheelEvent>


class RenderArea : public QWidget
{
    Q_OBJECT

public:

    int x,y;
    bool leftButton,rightButton;
    RenderArea(QWidget *parent = 0);
    RenderArea(int x, int y, QWidget *parent);
    ~RenderArea();
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    void setDim(int, int);

    QImage* sheet;

public slots:
    virtual void setAntialiased(bool antialiased);

signals:
    void leftButtonPressed();
    void leftButtonReleased();
    void leftDrag();
    void rightDrag();
    void rightButtonPressed();
    void rightButtonReleased();
    void mouseMove();
    void wheelMouse(int);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    bool antialiased;

};


class RatioRenderArea : public RenderArea
{
    Q_OBJECT

public:
    RatioRenderArea(double = 1.0, QWidget* = 0);

protected:
    virtual int heightForWidth( int w ) const { return w*ratio; }

private:
    double ratio;

};

#endif // RENDER_AREA_H
